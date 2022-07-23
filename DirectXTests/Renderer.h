#pragma once

#include <vector>
#include <stdint.h>

#include <d3d11.h>
#include "directxmath.h"

#include "Defines.h"

#include "ConstantBuffer.h"

class Graphics;
class Drawable;
class Pass;
class SpotLight;
class DirectionalLight;
class PointLight;
class Camera;
class Transform;


#define MAX_DIRLIGHTS   5
#define MAX_SPOTLIGHTS  5
#define MAX_POINTLIGHTS 5

enum CBuffSlots {
	DIRLIGHT_CBUFF_SLOT		= 0,
	POINTLIGHT_CBUFF_SLOT	= 1,
	SPOTLIGHT_CBUFF_SLOT	= 2,
	VIEW_CBUFF_SLOT			= 3,
	PROJ_CBUFF_SLOT			= 4,	
	MODEL_CBUFF_SLOT		= 5,
	BONES_CBUFF_SLOT		= 6,
	FREE_CBUFF_SLOT			= 7,
};

class Renderer {
private:
	struct Job {
		uint64_t key;
		const Drawable* drawable;
		const Transform* transform;
		Pass* pass;
	};

	struct CameraView {
		const Camera* camera;
		const Transform* transform;
	};

	struct alignas(16) DirLightData {
		POD::Vector4 color[MAX_DIRLIGHTS];
		POD::Vector4 dir[MAX_DIRLIGHTS];
		DirectX::XMMATRIX viewProj[MAX_DIRLIGHTS];
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

public:
	Renderer(Graphics& gfx) : 
		m_dirLightsCbuff(PixelConstantBuffer<DirLightData>(gfx, DIRLIGHT_CBUFF_SLOT)),
		m_pointLightsCbuff(PixelConstantBuffer<PointLightData>(gfx, POINTLIGHT_CBUFF_SLOT)),
		m_spotLightsCbuff(PixelConstantBuffer<SpotLightData>(gfx, SPOTLIGHT_CBUFF_SLOT))
	{}
	void SubmitDrawable(const Drawable* drawable, const Transform* transform, std::vector<Pass*> passes);
	void SubmitSpotlight(const SpotLight* spotlight, const Transform* worldTransform);
	void SubmitDirectionalLight(const DirectionalLight* spotlight, const Transform* worldTransform);
	void SubmitPointLight(const PointLight* spotlight, const Transform* worldTransform);
	void SubmitCamera(const Camera* camera, const Transform* worldTransform);
	void Render(Graphics& gfx);

	friend bool compareJob(const Job& j1, const Job& j2);

private:

	//bool IsInsideFrustum(const Camera& camera, const Transform& cameraTransform, const Drawable& drawable, const Transform& drawableTransform);

	std::vector<Job> m_jobs;
	std::vector<CameraView> m_cameras;

	DirLightData m_dirLightData;
	PointLightData m_pointLightData;
	SpotLightData m_spotLightData;
	PixelConstantBuffer<DirLightData> m_dirLightsCbuff;
	PixelConstantBuffer<PointLightData> m_pointLightsCbuff;
	PixelConstantBuffer<SpotLightData> m_spotLightsCbuff;
};