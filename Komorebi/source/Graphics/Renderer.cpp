#define NOMINMAX

#include <algorithm>
#include <iostream>
#include <sstream>
#include <ctime>

#define _XM_NO_INTRINSICS_

#include "Core/Engine.h"
#include "Core/Math/Transform.h"
#include "Core/Memory/Allocator.h"
#include "Entities/Drawable.h"
#include "Graphics/Pass.h"
#include "Graphics/Renderer.h"
#include "Entities/Light.h"
#include "Entities/Camera.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/Resource/Texture3D.h"
#include "Graphics/Material.h"

std::vector<DirectX::XMFLOAT4> getFrustumPlanes(DirectX::XMMATRIX&& viewProj);
void UpdateAABB(const Drawable::BVHData& a, const Transform& transform, Drawable::BVHData& b);
bool cullAABB(const std::vector<DirectX::XMFLOAT4>& frustumPlanes, const Drawable::BVHData& bvhData, const Transform* worldTransform);

Renderer::Renderer() :
	m_dirLightsCbuff(PixelConstantBuffer<DirLightData>(PCBUFF_DIRLIGHT_SLOT)),
	m_pointLightsCbuff(PixelConstantBuffer<PointLightData>(PCBUFF_POINTLIGHT_SLOT)),
	m_spotLightsCbuff(PixelConstantBuffer<SpotLightData>(PCBUFF_SPOTLIGHT_SLOT)),
	m_lightTransformCbuff(VertexConstantBuffer<LightTransformData>(VCBUFF_LIGHTTRANSFORM_SLOT)),
	m_shadowInfoCbuff(PixelConstantBuffer<ShadowInfoData>(PCBUFF_SHADOW_SLOT)),
	m_shadowMapSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, SRV_SHADOWMAP_SLOT),
	m_PCFFiltersSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, SRV_PCF_SLOT)
{
	// Initialize PCF filter random values
	int windowSize = 32;
	int filterSize = 5;
	int bufferSize = windowSize * windowSize * filterSize * filterSize * 2 * sizeof(float);
	float* randBuffer = (float*)malloc(bufferSize);
	std::srand(std::time(nullptr));
	int idx = 0;
	for (int i = 0; i < windowSize * windowSize; i++) {
		for (int u = 0; u < filterSize; u++) {
			for (int v = 0; v < filterSize; v++) {
				randBuffer[idx++] = float(u) - int(filterSize*0.5) + ((float)std::rand() / RAND_MAX);
				randBuffer[idx++] = float(v) - int(filterSize*0.5) + ((float)std::rand() / RAND_MAX);				
			}
		}
	}

	m_PCFFilters = memory::Factory::Create<Texture3D>((unsigned char*)randBuffer,
		filterSize * filterSize, 
		windowSize, 
		windowSize, 
		DXGI_FORMAT_R32G32_FLOAT, 8, SRV_PCF_SLOT);

	m_shadowInfoCbuff.SetBuffer(ShadowInfoData{
		1024.f,				// Shadowmap size
		1.0f/1024,			// 1/Shadowmap Size
		(float)windowSize,	
		(float)filterSize,	
		1.0f/filterSize		
	});
	m_shadowInfoCbuff.Update();

	m_renderTargets.push_back(Engine::GetDefaultRendertarget());
}

void Renderer::SubmitDrawable(const Drawable* drawable, const Transform* transform, Material* material) {
	for (Pass* pass : material->GetPasses()) {
		//Opaque:			29 empty + 1 culling + 2 layer + 8 matIdx + 8 passIdx + 16 depth
		//Transparent:		29 empty + 1 culling + 2 layer + 16 depth + 8 matIdx + 8 passIdx

		bool isTransparent = pass->m_layer == PASSLAYER_TRANSPARENT;

		// Pass layer
		uint64_t key = uint64_t(pass->m_layer) << 32;

		// Material (resource binds)
		unsigned int idx = material->GetIdx();
		unsigned int shifts = 8 + 16 * isTransparent;
		key |= ((uint64_t)idx) << shifts;

		// Pass index (state binds)
		shifts = !isTransparent * 16;
		key |= uint64_t(pass->GetIdx()) << shifts;

		m_jobs.push_back(Job{
			key,
			drawable,
			transform,
			pass,
			material
		});
	}
}

void Renderer::SubmitSpotlight(const SpotLight* spotlight, const Transform* worldTransform)
{	
	if (m_spotLightData.count >= MAX_DIRLIGHTS) return;

	const DirectX::SimpleMath::Vector3& c = spotlight->GetColor();
	const DirectX::SimpleMath::Vector3& f = worldTransform->GetForward();
	const DirectX::SimpleMath::Vector3& p = worldTransform->GetPositionUnsafe();

	m_spotLightData.m_color[m_spotLightData.count] = { c.x, c.y, c.z };
	m_spotLightData.dir[m_spotLightData.count] = { f.x, f.y, f.z };
	m_spotLightData.pos[m_spotLightData.count] = { p.x, p.y, p.z,};
	m_spotLightData.count++;
}

void Renderer::SubmitDirectionalLight(const DirectionalLight* dirlight, const Transform* worldTransform)
{
	if (m_dirLightData.count >= MAX_DIRLIGHTS) return;

	const DirectX::SimpleMath::Vector3& c = dirlight->GetColor();
	const DirectX::SimpleMath::Vector3& f = -worldTransform->GetForward();

	m_dirLightData.m_color[m_dirLightData.count] = { c.x, c.y, c.z};
	m_dirLightData.dir[m_dirLightData.count] = { f.x, f.y, f.z};

	m_lightTransformData.viewProj[m_dirLightData.count] = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixMultiply(
			worldTransform->GetInverseMatrixUnsafe(),
			dirlight->GetCamera()->getProj()
		)
	);

	// Shadow mapping camera
	m_cameras.push_back({ dirlight->GetCamera() , worldTransform });
	m_shadowMaps.push_back(dirlight->GetShadowMap());

	m_dirLightData.count++;
}

void Renderer::SubmitPointLight(const PointLight* pointlight, const Transform* worldTransform)
{
	if (m_pointLightData.count >= MAX_DIRLIGHTS) return;

	const DirectX::SimpleMath::Vector3& c = pointlight->GetColor();
	const DirectX::SimpleMath::Vector3& f = worldTransform->GetForward();

	m_pointLightData.m_color[m_pointLightData.count] = { c.x, c.y, c.z };
	m_pointLightData.pos[m_pointLightData.count] = { f.x, f.y, f.z };
	m_pointLightData.count++;
}

void Renderer::SubmitCamera(const Camera* camera, const Transform* worldTransform) {
	m_cameras.push_back({ camera, worldTransform });
}

bool compareJob(const Renderer::Job& j1, const Renderer::Job& j2) {
	return j1.key < j2.key; 
}
bool compareCamera(const Renderer::CameraView& c1, const Renderer::CameraView& c2) {
	return (c1.camera->m_priority < c2.camera->m_priority);
}

void Renderer::Render( ) {

	// Light info to pixel shader
	m_dirLightsCbuff.SetBuffer(m_dirLightData);
	m_pointLightsCbuff.SetBuffer(m_pointLightData);
	m_spotLightsCbuff.SetBuffer(m_spotLightData);

	m_dirLightsCbuff.Update ();
	m_pointLightsCbuff.Update ();
	m_spotLightsCbuff.Update ();

	m_dirLightsCbuff.Bind ();
	m_pointLightsCbuff.Bind ();
	m_spotLightsCbuff.Bind ();

	// Light transforms to vertex shader
	m_lightTransformCbuff.SetBuffer(m_lightTransformData);
	m_lightTransformCbuff.Update ();
	m_lightTransformCbuff.Bind ();

	// Cameras sorted by priority. Specially important for shadow mapping.
	std::sort(m_cameras.begin(), m_cameras.end(), compareCamera);

	for (int i = 0; i < m_cameras.size(); i++) {
		CameraView& camView = m_cameras[i];

		// Bind camera
		camView.camera->Bind ( camView.transform);		

		unsigned int jobsToExecute = m_jobs.size();

		// Get frustum planes for culling
		const std::vector<DirectX::XMFLOAT4>& planes = getFrustumPlanes(camView.transform->GetInverseMatrixUnsafe() * camView.camera->getProj());

		// Time to bind shadowmaps?
		if (i == m_shadowMaps.size() && i > 0) {
			for (Texture2D* rt : m_shadowMaps) {
				rt->Bind();
			}
			m_shadowMapSampler.Bind();
			m_PCFFilters->Bind();
			m_PCFFiltersSampler.Bind();
			m_shadowInfoCbuff.Bind();
		}

		// Update job sorting keys for this camera. 
		for (int i = 0; i < m_jobs.size(); i++) {		

			Job& job = m_jobs[i];

			// Ignored tag?
			bool ignore = false;//camView.camera->m_tagMask & job.drawable->m_tagMask;

			// Skybox passes are exempt from culling
			bool isSkybox = job.pass->m_layer == PASSLAYER_SKYBOX; 

			// Frustum culling
			bool cull = ignore || (!isSkybox && cullAABB(planes, job.drawable->GetBVHData(), job.transform));

			job.key &= ~(uint64_t(1) << 34);
			job.key |= uint64_t(cull) << 34;

			jobsToExecute -= cull;

			// Depth
			if (!cull) {
				bool isTransparent = job.pass->m_layer == PASSLAYER_TRANSPARENT;
				unsigned int shifts = isTransparent * 16;
				job.key &= ~(uint64_t(0xFFFF) << shifts);
				float depth = camView.transform->PointToLocalUnsafe(job.transform->GetPositionUnsafe()).Length();
				float farZ = camView.camera->m_far;
				float nearZ = camView.camera->m_near;
				float normalizedDepth = (depth - nearZ) / (farZ - nearZ);
				uint64_t depthKeyComponent = (isTransparent ? 1.0f - normalizedDepth : normalizedDepth) * 0xFFFF;
				job.key |= depthKeyComponent << shifts;
				job.key |= 0;
			}					
			
		}

		// Sort jobs based on its key
		std::sort(m_jobs.begin(), m_jobs.end(), compareJob);

		// Dispatch jobs
		int stateBindCount = 0;
		int resourceBindCount = 0;
		Pass* lastPass = nullptr;
		Material* lastMat = nullptr;
		for (int i = 0; i < jobsToExecute; i++) {

			Job job = m_jobs[i];

			Pass* nextPass = i < jobsToExecute - 1 ? m_jobs[i+1].pass : nullptr;
			Material* nextMat = i < jobsToExecute - 1 ? m_jobs[i+1].material : nullptr;

			if (lastPass != job.pass) { job.pass->Bind (); stateBindCount++; };
			if (lastMat != job.material) { job.material->Bind (); resourceBindCount++; };

			const DirectX::XMMATRIX& modelMat = DirectX::XMMatrixTranspose(job.transform->GetMatrix());
			job.drawable->Draw (modelMat);

			if (nextPass != job.pass) job.pass->Unbind();
			if (nextMat != job.material) job.material->Unbind();

			lastPass = job.pass;
			lastMat = job.material;
		}
		
		std::ostringstream os_;
		os_ << "setPass: " << stateBindCount << 
			  " setMat: " << resourceBindCount << 
			  " DrawCalls: " << jobsToExecute << "\n";
		OutputDebugString( os_.str().c_str());
		
		camView.camera->Unbind ();
	}

	for (Texture2D* rt : m_shadowMaps) {
		rt->Unbind();
	}

	m_jobs.clear();
	m_cameras.clear();
	m_shadowMaps.clear();
	m_dirLightData.count = 0;
	m_pointLightData.count = 0;
	m_spotLightData.count = 0;
}

bool cullAABB(const std::vector<DirectX::XMFLOAT4>& frustumPlanes, const Drawable::BVHData& bvhData, const Transform* worldTransform) {

	Drawable::BVHData updatedBvh;
	UpdateAABB(bvhData, *worldTransform, updatedBvh);
	for (int planeID = 0; planeID < 6; ++planeID)
	{
		DirectX::XMVECTOR planeNormal{ frustumPlanes[planeID].x, frustumPlanes[planeID].y, frustumPlanes[planeID].z, 0.0f };
		float planeConstant = frustumPlanes[planeID].w;

		DirectX::SimpleMath::Vector3 worldMin = updatedBvh.m_min + worldTransform->GetPositionUnsafe();
		DirectX::SimpleMath::Vector3 worldMax = updatedBvh.m_max + worldTransform->GetPositionUnsafe();

		// Check each axis (x,y,z) to get the AABB vertex furthest away from the direction the plane is facing (plane normal)
		DirectX::XMFLOAT3 axisVert;

		// x-axis
		if (frustumPlanes[planeID].x < 0.0f)    // Which AABB vertex is furthest down (plane normals direction) the x axis
			axisVert.x = worldMin.x; // min x plus tree positions x
		else
			axisVert.x = worldMax.x; // max x plus tree positions x

		// y-axis
		if (frustumPlanes[planeID].y < 0.0f)    // Which AABB vertex is furthest down (plane normals direction) the y axis
			axisVert.y = worldMin.y; // min y plus tree positions y
		else
			axisVert.y = worldMax.y; // max y plus tree positions y

		// z-axis
		if (frustumPlanes[planeID].z < 0.0f)    // Which AABB vertex is furthest down (plane normals direction) the z axis
			axisVert.z = worldMin.z; // min z plus tree positions z
		else
			axisVert.z = worldMax.z; // max z plus tree positions z

		// Now we get the signed distance from the AABB vertex that's furthest down the frustum planes normal,
		// and if the signed distance is negative, then the entire bounding box is behind the frustum plane, which means
		// that it should be culled
		if (XMVectorGetX(XMVector3Dot(planeNormal, XMLoadFloat3(&axisVert))) + planeConstant < 0.0f)
		{
			return true;
		}
	}
	return false;
}

std::vector<DirectX::XMFLOAT4> getFrustumPlanes(DirectX::XMMATRIX&& viewProj)
{
	// x, y, z, and w represent A, B, C and D in the plane equation
	// where ABC are the xyz of the planes normal, and D is the plane constant
	std::vector<DirectX::XMFLOAT4> tempFrustumPlane(6);

	// Left Frustum Plane
	// Add first column of the matrix to the fourth column
	tempFrustumPlane[0].x = viewProj._14 + viewProj._11;
	tempFrustumPlane[0].y = viewProj._24 + viewProj._21;
	tempFrustumPlane[0].z = viewProj._34 + viewProj._31;
	tempFrustumPlane[0].w = viewProj._44 + viewProj._41;

	// Right Frustum Plane
	// Subtract first column of matrix from the fourth column
	tempFrustumPlane[1].x = viewProj._14 - viewProj._11;
	tempFrustumPlane[1].y = viewProj._24 - viewProj._21;
	tempFrustumPlane[1].z = viewProj._34 - viewProj._31;
	tempFrustumPlane[1].w = viewProj._44 - viewProj._41;

	// Top Frustum Plane
	// Subtract second column of matrix from the fourth column
	tempFrustumPlane[2].x = viewProj._14 - viewProj._12;
	tempFrustumPlane[2].y = viewProj._24 - viewProj._22;
	tempFrustumPlane[2].z = viewProj._34 - viewProj._32;
	tempFrustumPlane[2].w = viewProj._44 - viewProj._42;

	// Bottom Frustum Plane
	// Add second column of the matrix to the fourth column
	tempFrustumPlane[3].x = viewProj._14 + viewProj._12;
	tempFrustumPlane[3].y = viewProj._24 + viewProj._22;
	tempFrustumPlane[3].z = viewProj._34 + viewProj._32;
	tempFrustumPlane[3].w = viewProj._44 + viewProj._42;

	// Near Frustum Plane
	// We could add the third column to the fourth column to get the near plane,
	// but we don't have to do this because the third column IS the near plane
	tempFrustumPlane[4].x = viewProj._13;
	tempFrustumPlane[4].y = viewProj._23;
	tempFrustumPlane[4].z = viewProj._33;
	tempFrustumPlane[4].w = viewProj._43;

	// Far Frustum Plane
	// Subtract third column of matrix from the fourth column
	tempFrustumPlane[5].x = viewProj._14 - viewProj._13;
	tempFrustumPlane[5].y = viewProj._24 - viewProj._23;
	tempFrustumPlane[5].z = viewProj._34 - viewProj._33;
	tempFrustumPlane[5].w = viewProj._44 - viewProj._43;

	// Normalize plane normals (A, B and C (xyz))
	// Also take note that planes face inward
	for (int i = 0; i < 6; ++i)
	{
		float length = sqrt((tempFrustumPlane[i].x * tempFrustumPlane[i].x) + (tempFrustumPlane[i].y * tempFrustumPlane[i].y) + (tempFrustumPlane[i].z * tempFrustumPlane[i].z));
		tempFrustumPlane[i].x /= length;
		tempFrustumPlane[i].y /= length;
		tempFrustumPlane[i].z /= length;
		tempFrustumPlane[i].w /= length;
	}

	return tempFrustumPlane;
}

// Transform AABB a by the matrix m and translation t,
// find maximum extents, and store result into AABB b.
void UpdateAABB(const Drawable::BVHData& a, const Transform& transform, Drawable::BVHData& b)
{
	const float* amin = (const float*)&a.m_min.x;
	const float* amax = (const float*)&a.m_max.x;
	float* bmin = (float*)&b.m_min.x;
	float* bmax = (float*)&b.m_max.x;
	float* t = (float*)&transform.GetPositionUnsafe();
	const DirectX::XMMATRIX& m = transform.GetMatrix();
	// For all three axes
	for (int i = 0; i < 3; i++) {
		// Start by adding in translation
		bmin[i] = bmax[i] = t[i];
		// Form extent by summing smaller and larger terms respectively
		for (int j = 0; j < 3; j++) {
			float e = m(j,i) * amin[j];
			float f = m(j,i) * amax[j];
			if (e < f) {
				bmin[i] += e;
				bmax[i] += f;
			}
			else {
				bmin[i] += f;
				bmax[i] += e;
			}
		}
	}
}