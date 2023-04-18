#pragma once

#include <vector>
#include <stdint.h>

#include <d3d11.h>
#include "directxmath.h"

#include "Graphics/BindableSlotsInfo.h"
#include "Core/Defines.h"
#include "Graphics/Bindables/Resource/ConstantBuffer.h"
#include "Graphics/Bindables/State/SamplerState.h"

class SpotLight;
class DirectionalLight;
class PointLight;
class Camera;
class Transform;
class Drawable;

namespace gfx {

class Graphics;
class Pass;
class Texture2D;
class Texture3D;
class Material;
class RenderInfo;

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

struct LightView {
	const Camera* m_camera;
	const Transform* m_transform;
	const RenderTarget* m_rt;
};

class Renderer {
private:
	
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

	void Init();

	void SubmitDrawable(const Drawable* drawable, const Transform* transform, Material* material);
	void SubmitSpotlight(const SpotLight* spotlight, const Transform* worldTransform);
	void SubmitDirectionalLight(const DirectionalLight* spotlight, const Transform* worldTransform);
	void SubmitPointLight(const PointLight* spotlight, const Transform* worldTransform);
	void SubmitCamera(const Camera* camera, const Transform* worldTransform);

	void Render();
	void Clear();

	const std::vector<Job>& GetJobs() const { return m_jobs; }

	const Drawable* GetQuadPrimitive() const;

	RenderTarget* GetGlobalRenderTarget(unsigned int idx) const { return m_renderTargets[idx]; }

	RenderInfo* GetRenderInfo() const { return m_renderInfo; }

	const std::vector<const DirectionalLight*>& GetDirLights() {return m_dirLights;}
	const std::vector<const SpotLight*>& GetSpotLights() {return m_spotLights;}
	const std::vector<const PointLight*>& GetPointLights() {return m_pointLights;}

private:

	std::vector<Job> m_jobs;
	std::vector<CameraView> m_camViews;
	std::vector<LightView> m_lightViews;

	SamplerState m_shadowMapSampler;
	SamplerState m_PCFFiltersSampler;
	Texture3D* m_PCFFilters;
	PixelConstantBuffer<ShadowInfoData> m_shadowInfoCbuff;

	std::vector<const DirectionalLight*> m_dirLights;
	std::vector<const SpotLight*> m_spotLights;
	std::vector<const PointLight*> m_pointLights;

	std::vector<RenderTarget*> m_renderTargets;

	RenderInfo* m_renderInfo = nullptr;
};

}