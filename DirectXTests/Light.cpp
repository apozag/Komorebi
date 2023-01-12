#include "Light.h"
#include "Transform.h"
#include "Renderer.h"

void DirectionalLight::Setup() {
	m_rt = new RenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = new Camera(1.0472f, 1, 0.1, 1000, m_rt, true);
	m_camera->m_priority = -100;
}

void SpotLight::Setup() {

	m_rt = new RenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = new Camera(1.0472f, 1, 0.1, 1000, m_rt, true);
	m_camera->m_priority = -100;
}

void PointLight::Setup() {

	m_rts[0] = RenderTarget(512, 512, DXGI_FORMAT_A8_UNORM, 0, 2);
	m_rts[1] = RenderTarget(512, 512, DXGI_FORMAT_A8_UNORM, 0, 2);
	m_rts[2] = RenderTarget(512, 512, DXGI_FORMAT_A8_UNORM, 0, 2);
	m_rts[3] = RenderTarget(512, 512, DXGI_FORMAT_A8_UNORM, 0, 2);
	m_rts[4] = RenderTarget(512, 512, DXGI_FORMAT_A8_UNORM, 0, 2);
	m_rts[5] = RenderTarget(512, 512, DXGI_FORMAT_A8_UNORM, 0, 2);

	m_rts[0].Setup();
	m_rts[1].Setup();
	m_rts[2].Setup();
	m_rts[3].Setup();
	m_rts[4].Setup();
	m_rts[5].Setup();
	
	m_cameras[0] = Camera(1.0472f, 1, 0.1, 1000, &m_rts[0]);
	m_cameras[1] = Camera(1.0472f, 1, 0.1, 1000, &m_rts[1]);
	m_cameras[2] = Camera(1.0472f, 1, 0.1, 1000, &m_rts[2]);
	m_cameras[3] = Camera(1.0472f, 1, 0.1, 1000, &m_rts[3]);
	m_cameras[4] = Camera(1.0472f, 1, 0.1, 1000, &m_rts[4]);
	m_cameras[5] = Camera(1.0472f, 1, 0.1, 1000, &m_rts[5]);
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