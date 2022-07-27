#include "Light.h"
#include "Transform.h"
#include "Renderer.h"

DirectionalLight::DirectionalLight(Graphics& gfx, DirectX::SimpleMath::Vector3 color) :
	color(color)
{
	m_rt = new RenderTarget(gfx, 1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, TEX2D_SHADOWMAP_SLOT);
	m_camera = new Camera(gfx, 1.0472f, 1, 0.1, 1000, m_rt, true);
	m_camera->m_priority = -1;
}

SpotLight::SpotLight(Graphics& gfx, DirectX::SimpleMath::Vector3 color, float phi, float theta) :
	color(color),
	phi(phi),
	theta(theta),
	m_rt(RenderTarget(gfx, 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2)),
	m_camera(Camera(gfx, 1.0472f, 1, 0.1, 1000, &m_rt))
{}

PointLight::PointLight(Graphics& gfx, DirectX::SimpleMath::Vector3 color) : 
	color(color),
	m_rts{
			RenderTarget(gfx, 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget(gfx, 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget(gfx, 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget(gfx, 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget(gfx, 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget(gfx, 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2)
	},
	m_cameras{
			 Camera(gfx, 1.0472f, 1, 0.1, 1000, &m_rts[0]),
			 Camera(gfx, 1.0472f, 1, 0.1, 1000, &m_rts[1]),
			 Camera(gfx, 1.0472f, 1, 0.1, 1000, &m_rts[2]),
			 Camera(gfx, 1.0472f, 1, 0.1, 1000, &m_rts[3]),
			 Camera(gfx, 1.0472f, 1, 0.1, 1000, &m_rts[4]),
			 Camera(gfx, 1.0472f, 1, 0.1, 1000, &m_rts[5])
	}
{}

void DirectionalLight::Insert(Renderer& renderer, const Transform& worldTransform) {
	renderer.SubmitDirectionalLight(this, &worldTransform);
}

void SpotLight::Insert(Renderer& renderer, const Transform& worldTransform)
{
	renderer.SubmitSpotlight(this, &worldTransform);
}

void PointLight::Insert(Renderer& renderer, const Transform& worldTransform) {
	renderer.SubmitPointLight(this, &worldTransform);
}


