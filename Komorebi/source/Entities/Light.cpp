#include "Entities/Light.h"
#include "Core/Math/Transform.h"
#include "Graphics/Renderer.h"

void DirectionalLight::Setup() {
	m_rt = new RenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = new Camera(1.0472f, 1, 0.1, 1000, m_rt, true);	
	m_camera->m_priority = -100;
	m_camera->Setup();
}

void SpotLight::Setup() {

	m_rt = new RenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = new Camera(1.0472f, 1, 0.1, 1000, m_rt, true);
	m_camera->m_priority = -100;
	m_camera->Setup();
}

void PointLight::Setup() {
	for (int i = 0; i < 6; i++) {
		m_rts[i] = RenderTarget(512, 512, DXGI_FORMAT_A8_UNORM, 0, 2);
		m_rts[i].Setup();
		m_cameras[i] = Camera(1.0472f, 1, 0.1, 1000, &m_rts[i]);
		m_cameras[i].Setup();
	}
}

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

REFLECT_STRUCT_BEGIN(DirectionalLight, Entity)
REFLECT_STRUCT_MEMBER(m_color)
REFLECT_STRUCT_END(DirectionalLight)

REFLECT_STRUCT_BEGIN(PointLight, Entity)
REFLECT_STRUCT_MEMBER(m_color)
REFLECT_STRUCT_END(PointLight)

REFLECT_STRUCT_BEGIN(SpotLight, Entity)
REFLECT_STRUCT_MEMBER(m_color)
REFLECT_STRUCT_MEMBER(m_phi)
REFLECT_STRUCT_MEMBER(m_theta)
REFLECT_STRUCT_END(SpotLight)