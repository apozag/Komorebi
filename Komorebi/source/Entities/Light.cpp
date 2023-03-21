#include "Entities/Light.h"
#include "Scene/Scene.h"
#include "Core/Memory/Allocator.h"
#include "Core/Math/Transform.h"
#include "Graphics/Renderer.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"

void DirectionalLight::Setup() {
	m_rt = memory::Factory::Create<RenderTarget>(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = memory::Factory::Create<Camera>(1.0472f, 1, 0.1f, 1000, m_rt, true);
	m_camera->m_priority = -100;
	m_camera->Setup();
}

void SpotLight::Setup() {

	m_rt = memory::Factory::Create<RenderTarget>(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = memory::Factory::Create<Camera>(1.0472f, 1, 0.1f, 1000, m_rt, true);
	m_camera->m_priority = -100;
	m_camera->Setup();
}

void PointLight::Setup() {
	for (int i = 0; i < 6; i++) {
		m_rts[i] = RenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
		m_rts[i].Setup();
		m_cameras[i] = Camera(1.0472f, 1, 0.1f, 1000, &m_rts[i]);
		m_cameras[i].Setup();
	}
}

DirectionalLight::~DirectionalLight() {
	//delete(m_rt);
	//delete(m_camera);
}

SpotLight::~SpotLight() {
	//delete(m_rt);
	//delete(m_camera);
}

PointLight::~PointLight() {
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

REGISTER_ENTITY(DirectionalLight)
REGISTER_ENTITY(PointLight)
REGISTER_ENTITY(SpotLight)
