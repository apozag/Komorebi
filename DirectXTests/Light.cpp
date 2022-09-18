#include "Light.h"
#include "Transform.h"
#include "Renderer.h"

DirectionalLight::DirectionalLight( DirectX::SimpleMath::Vector3 color) :
	color(color)
{
	m_rt = new RenderTarget ( 1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, TEX2D_SHADOWMAP_SLOT);
	m_camera = new Camera ( 1.0472f, 1, 0.1, 1000, m_rt, true);
	m_camera->m_priority = -100;
}

SpotLight::SpotLight( DirectX::SimpleMath::Vector3 color, float phi, float theta) :
	color(color),
	phi(phi),
	theta(theta),
	m_rt(RenderTarget ( 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2)),
	m_camera(Camera ( 1.0472f, 1, 0.1, 1000, &m_rt))
{}

PointLight::PointLight( DirectX::SimpleMath::Vector3 color) : 
	color(color),
	m_rts{
			RenderTarget ( 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget ( 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget ( 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget ( 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget ( 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2),
			RenderTarget ( 512, 512, DXGI_FORMAT_A8_UNORM, 0, 2)
	},
	m_cameras{
			 Camera ( 1.0472f, 1, 0.1, 1000, &m_rts[0]),
			 Camera ( 1.0472f, 1, 0.1, 1000, &m_rts[1]),
			 Camera ( 1.0472f, 1, 0.1, 1000, &m_rts[2]),
			 Camera ( 1.0472f, 1, 0.1, 1000, &m_rts[3]),
			 Camera ( 1.0472f, 1, 0.1, 1000, &m_rts[4]),
			 Camera ( 1.0472f, 1, 0.1, 1000, &m_rts[5])
	}
{}

void DirectionalLight::Insert(Node* node, const Transform& worldTransform) {
	GetRenderer()->SubmitDirectionalLight(this, &worldTransform);
}

void SpotLight::Insert(Node* node, const Transform& worldTransform)
{
	GetRenderer()->SubmitSpotlight(this, &worldTransform);
}

void PointLight::Insert(Node* node, const Transform& worldTransform) {
	GetRenderer()->SubmitPointLight(this, &worldTransform);
}


