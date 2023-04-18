#include "Entities/Light.h"
#include "Scene/Scene.h"
#include "Core/Memory/Factory.h"
#include "Core/Math/Transform.h"
#include "Graphics/Renderer.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Graphics/Bindables/Resource/Texture2D.h"

void DirectionalLight::Setup() {
	m_rt = memory::Factory::Create<gfx::RenderTarget>(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = memory::Factory::Create<Camera>(1.0472f, 1, 0.1f, 1000, true);
	m_camera->Setup();
}

void SpotLight::Setup() {
	m_rt = memory::Factory::Create<gfx::RenderTarget>(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT);
	m_rt->Setup();
	m_camera = memory::Factory::Create<Camera>(1.0472f, 1, 0.1f, 1000);
	m_camera->Setup();	
}

void PointLight::Setup() {
	for (int i = 0; i < 6; i++) {
		m_rts[i] = gfx::RenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, 0, SRV_SHADOWMAP_SLOT + i);
		m_rts[i].Setup();
		m_cameras[i] = Camera(1.0472f, 1, 0.1f, 1000);
		m_cameras[i].Setup();
	}
}

DirectionalLight::~DirectionalLight() {
}

SpotLight::~SpotLight() {
}

PointLight::~PointLight() {
}

void DirectionalLight::Insert(Node* node, const Transform& worldTransform) {
	const DirectX::SimpleMath::Vector3& dir = -worldTransform.GetForward();
	m_pcbuffer.m_buffer.m_color = { m_color.x, m_color.y, m_color.z };
	m_pcbuffer.m_buffer.m_dir = { dir.x, dir.y, dir.z, 0.f };
	m_vcbuffer.m_buffer.m_viewProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(worldTransform.GetInverseMatrixUnsafe(),m_camera->getProj()));

	m_pcbuffer.Update();
	m_vcbuffer.Update();

	GetRenderer()->SubmitDirectionalLight(this, &worldTransform);
}

void SpotLight::Insert(Node* node, const Transform& worldTransform)
{
	const DirectX::SimpleMath::Vector3& f = worldTransform.GetForward();
	const DirectX::SimpleMath::Vector3& p = worldTransform.GetPositionUnsafe();

	m_pcbuffer.m_buffer.m_color = { m_color.x, m_color.y, m_color.z, 0.f };
	m_pcbuffer.m_buffer.m_dir = { f.x, f.y, f.z, 0.f};
	m_pcbuffer.m_buffer.m_pos = { p.x, p.y, p.z, 0.f};
	m_pcbuffer.m_buffer.m_phiTheta = { m_phi, m_theta, 0.f, 0.f };
	m_vcbuffer.m_buffer.m_viewProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(worldTransform.GetInverseMatrixUnsafe(), m_camera->getProj()));

	m_pcbuffer.Update();
	m_vcbuffer.Update();

	GetRenderer()->SubmitSpotlight(this, &worldTransform);
}

void PointLight::Insert(Node* node, const Transform& worldTransform) {
	GetRenderer()->SubmitPointLight(this, &worldTransform);
}

void DirectionalLight::Bind() const{
	m_pcbuffer.Bind();
	m_vcbuffer.Bind();
	m_rt->GetTextures2D()[0]->Bind();
}
void SpotLight::Bind() const{
	m_pcbuffer.Bind();
	m_vcbuffer.Bind();
	m_rt->GetTextures2D()[0]->Bind();
}
void PointLight::Bind() const{
	m_pcbuffer.Bind();
	m_vcbuffer.Bind();
	for (auto rt : m_rts) {
		rt.GetTextures2D()[0]->Bind();
	}
}

void DirectionalLight::Unbind() const {
	m_pcbuffer.Unbind();
	m_vcbuffer.Unbind();
	m_rt->GetTextures2D()[0]->Unbind();
}
void SpotLight::Unbind() const {
	m_pcbuffer.Unbind();
	m_vcbuffer.Unbind();
	m_rt->GetTextures2D()[0]->Unbind();
}
void PointLight::Unbind() const {
	m_pcbuffer.Unbind();
	m_vcbuffer.Unbind();
	for (auto rt : m_rts) {
		rt.GetTextures2D()[0]->Unbind();
	}
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
