#pragma once

#include <vector>
#include <stdint.h>

#include <d3d11.h>
#include "directxmath.h"

#include "BindableSlotsInfo.h"
#include "Defines.h"
#include "ConstantBuffer.h"
#include "Sampler.h"

class Graphics;
class Drawable;
class Pass;
class SpotLight;
class DirectionalLight;
class PointLight;
class Camera;
class Transform;
class Texture2D;
class Texture3D;
class Material;

class Renderer {
private:
	struct Job {
		uint64_t key;
		const Drawable* drawable;
		const Transform* transform;
		Pass* pass;
		Material* material;
	};

	struct CameraView {
		const Camera* camera;
		const Transform* transform;
	};

	struct alignas(16) DirLightData {
		POD::Vector4 color[MAX_DIRLIGHTS];
		POD::Vector4 dir[MAX_DIRLIGHTS];
		unsigned int count;
	};
	struct alignas(16) PointLightData {
		POD::Vector3 pos[MAX_POINTLIGHTS];
		POD::Vector3 color[MAX_POINTLIGHTS];
		unsigned int count;
	};
	struct alignas(16) SpotLightData {
		POD::Vector3 pos[MAX_SPOTLIGHTS];
		POD::Vector3 dir[MAX_SPOTLIGHTS];
		POD::Vector3 color[MAX_SPOTLIGHTS];
		unsigned int count;
	};
	struct alignas(16) LightTransformData {
		DirectX::XMMATRIX viewProj[MAX_DIRLIGHTS];
	};
	struct alignas(16) ShadowInfoData {
		float shadowmapSize;
		float shadowmapTexelSize;
		float pcfWindowSize;
		float pcfFilterSize;
		float pcfFilterSizeInv;
		int a, b, c;
	};

public:
	Renderer();
	void SubmitDrawable(const Drawable* drawable, const Transform* transform, Material* material);
	void SubmitSpotlight(const SpotLight* spotlight, const Transform* worldTransform);
	void SubmitDirectionalLight(const DirectionalLight* spotlight, const Transform* worldTransform);
	void SubmitPointLight(const PointLight* spotlight, const Transform* worldTransform);
	void SubmitCamera(const Camera* camera, const Transform* worldTransform);
	void Render( );

	friend bool compareJob(const Job& j1, const Job& j2);
	friend bool compareCamera(const CameraView& c1, const CameraView& c2);

private:

	std::vector<Job> m_jobs;
	std::vector<CameraView> m_cameras;

	DirLightData m_dirLightData;
	PointLightData m_pointLightData;
	SpotLightData m_spotLightData;
	PixelConstantBuffer<DirLightData> m_dirLightsCbuff;
	PixelConstantBuffer<PointLightData> m_pointLightsCbuff;
	PixelConstantBuffer<SpotLightData> m_spotLightsCbuff;	

	LightTransformData m_lightTransformData;
	VertexConstantBuffer<LightTransformData> m_lightTransformCbuff;

	std::vector<Texture2D*> m_shadowMaps;
	Sampler m_shadowMapSampler;
	Sampler m_PCFFiltersSampler;
	Texture3D* m_PCFFilters;
	PixelConstantBuffer<ShadowInfoData> m_shadowInfoCbuff;
};