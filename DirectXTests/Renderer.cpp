#include <algorithm>
#include <iostream>
#include <sstream>

#define _XM_NO_INTRINSICS_

#include "Pass.h"
#include "Renderer.h"
#include "Light.h"
#include "Camera.h"
#include "RenderTarget.h"

std::vector<DirectX::XMFLOAT4> getFrustumPlanes(DirectX::XMMATRIX&& viewProj);
bool cullAABB(std::vector<DirectX::XMFLOAT4>&& frustumPlanes, const Drawable::BVHData& bvhData, const Transform* worldTransform);

void Renderer::SubmitDrawable(const Drawable* drawable, const Transform* transform, std::vector<Pass*> passes) {
	for (Pass* pass : passes) {
		//Opaque:			29 empty + 1 culling + 2 layer + 8 matIdx + 8 passIdx + 16 depth
		//Transparent:		29 empty + 1 culling + 2 layer + 16 depth + 8 matIdx + 8 passIdx						
		m_jobs.push_back(Job{
			0,
			drawable,
			transform,
			pass
		});
	}
}

void Renderer::SubmitSpotlight(const SpotLight* spotlight, const Transform* worldTransform)
{	
	if (m_spotLightData.count >= MAX_DIRLIGHTS) return;

	const DirectX::SimpleMath::Vector3& c = spotlight->GetColor();
	const DirectX::SimpleMath::Vector3& f = worldTransform->GetForward();
	const DirectX::SimpleMath::Vector3& p = worldTransform->GetPositionUnsafe();

	m_spotLightData.color[m_spotLightData.count] = { c.x, c.y, c.z };
	m_spotLightData.dir[m_spotLightData.count] = { f.x, f.y, f.z };
	m_spotLightData.pos[m_spotLightData.count] = { p.x, p.y, p.z,};
	m_spotLightData.count++;				  
}

void Renderer::SubmitDirectionalLight(const DirectionalLight* dirlight, const Transform* worldTransform)
{
	if (m_dirLightData.count >= MAX_DIRLIGHTS) return;

	const DirectX::SimpleMath::Vector3& c = dirlight->GetColor();
	const DirectX::SimpleMath::Vector3& f = -worldTransform->GetForward();

	m_dirLightData.color[m_dirLightData.count] = { c.x, c.y, c.z};
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

	m_pointLightData.color[m_pointLightData.count] = { c.x, c.y, c.z };
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
	return c1.camera->m_priority < c2.camera->m_priority;
}

void Renderer::Render(Graphics& gfx) {

	// Light info to pixel shader
	m_dirLightsCbuff.SetBuffer(m_dirLightData);
	m_pointLightsCbuff.SetBuffer(m_pointLightData);
	m_spotLightsCbuff.SetBuffer(m_spotLightData);

	m_dirLightsCbuff.Update(gfx);
	m_pointLightsCbuff.Update(gfx);
	m_spotLightsCbuff.Update(gfx);

	m_dirLightsCbuff.Bind(gfx);
	m_pointLightsCbuff.Bind(gfx);
	m_spotLightsCbuff.Bind(gfx);

	// Light transforms to vertex shader
	m_lightTransformCbuff.SetBuffer(m_lightTransformData);
	m_lightTransformCbuff.Update(gfx);
	m_lightTransformCbuff.Bind(gfx);

	// Shadowmap shader resources bound at the begginning. 
	// When bound as render target, they will be bound back as srv when rendering is done.
	for (RenderTarget* rt : m_shadowMaps) {
		rt->Bind(gfx);
	}
	m_shadowMapSampler.Bind(gfx);

	// Cameras sorted by priority. Specially important for shadow mapping.
	std::sort(m_cameras.begin(), m_cameras.end(), compareCamera);

	for (CameraView camView : m_cameras) {		

		// Bind camera
		camView.camera->Bind(gfx, camView.transform);		

		int jobsToExecute = m_jobs.size();

		// Update job sorting keys for this camera. 
		// Some parts are independent from the camera, but oh well.
		for (int i = 0; i < m_jobs.size(); i++) {

			m_jobs[i].key = uint64_t(m_jobs[i].pass->layer) << 32;

			bool isTransparent = m_jobs[i].pass->layer == PASSLAYER_TRANSPARENT;

			int shifts = isTransparent ? 0 : 16;
			m_jobs[i].key |= uint64_t(m_jobs[i].pass->layer) << shifts;

			// depth
			shifts =  isTransparent? 16 : 0;
			m_jobs[i].key &= ~(uint64_t(0xFFFF) << shifts);
			float depth = camView.transform->PointToLocalUnsafe(m_jobs[i].transform->GetPositionUnsafe()).Length();
			float normalizedDepth = (depth - camView.camera->m_near) / (camView.camera->m_far - camView.camera->m_near);
			uint64_t depthKeyComponent = (isTransparent? 1.0f-normalizedDepth : normalizedDepth) * 0xFFFF;
			m_jobs[i].key |= depthKeyComponent << shifts;

			// frustum culling			
			bool cull = m_jobs[i].pass->layer != PASSLAYER_SKYBOX && 
				cullAABB(
					getFrustumPlanes(camView.transform->GetInverseMatrixUnsafe() * camView.camera->getProj()),
					m_jobs[i].drawable->GetBVHData(), 
					m_jobs[i].transform
				);
			m_jobs[i].key &= ~(uint64_t(1) << 34);
			m_jobs[i].key |= uint64_t(cull) << 34;
			jobsToExecute -= cull;
			
		}

		// Jobs sorted in an attempt to minimize state changes
		std::sort(m_jobs.begin(), m_jobs.end(), compareJob);

		// Dispatch jobs
		int bindCount = 0;
		for (int i = 0; i < jobsToExecute; i++) {

			Job job = m_jobs[i];

			Pass* lastPass = i > 0 ? m_jobs[i-1].pass : nullptr;
			Pass* nextPass = i < jobsToExecute - 1 ? m_jobs[i+1].pass : nullptr;

			job.drawable->m_modelCbuffer->SetBuffer({ DirectX::XMMatrixTranspose(job.transform->GetMatrix()) });

			if (lastPass != job.pass) { job.pass->Bind(gfx); bindCount++; };
			job.drawable->Draw(gfx);
			if (nextPass != job.pass)  job.pass->Unbind(gfx);
		}
		/*
		std::ostringstream os_;
		os_ << "setPass: " << bindCount << " DrawCalls: " << jobsToExecute << "\n";
		OutputDebugString( os_.str().c_str());
		*/
		camView.camera->Unbind(gfx);
	}
	m_jobs.clear();
	m_cameras.clear();
	m_shadowMaps.clear();
	m_dirLightData.count = 0;
	m_pointLightData.count = 0;
	m_spotLightData.count = 0;
}

bool cullAABB(std::vector<DirectX::XMFLOAT4>&& frustumPlanes, const Drawable::BVHData& bvhData, const Transform* worldTransform) {
	for (int planeID = 0; planeID < 6; ++planeID)
	{
		DirectX::XMVECTOR planeNormal{ frustumPlanes[planeID].x, frustumPlanes[planeID].y, frustumPlanes[planeID].z, 0.0f };
		float planeConstant = frustumPlanes[planeID].w;

		DirectX::SimpleMath::Vector3 worldMin = worldTransform->PointToWorld(bvhData.min);
		DirectX::SimpleMath::Vector3 worldMax = worldTransform->PointToWorld(bvhData.max);

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
