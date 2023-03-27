#include "Graphics/BindableSlotsInfo.h"

#include "Core/Memory/Factory.h"
#include "Entities/Camera.h"
#include "Scene\Scene.h"
#include "Core/Math/Transform.h"
#include "Graphics/Bindables/Resource/ConstantBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"

void Camera::Setup() {

	if (m_orthographic) {
		m_proj = DirectX::XMMatrixOrthographicLH(500, 500, m_near, m_far);
	}
	else {
		m_proj = DirectX::XMMatrixPerspectiveFovLH(m_fov, m_aspectratio, m_near, 1000);
	}
	m_cameraTransformCB = memory::Factory::Create<gfx::VertexConstantBuffer<CameraTransformCB>>(VCBUFF_CAMERATRANSFORM_SLOT);
	if(!m_rt) m_rt = GetRenderer()->GetRenderTarget(m_RTId);
}

void Camera::Bind( const Transform* worldTransform) const {
	const DirectX::XMMATRIX view = worldTransform->GetInverseMatrixUnsafe();
	DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, m_proj);
	m_cameraTransformCB->SetBuffer(CameraTransformCB{
		DirectX::XMMatrixTranspose(viewProj),
		DirectX::XMMatrixTranspose(view),
		DirectX::XMMatrixTranspose(m_proj)
		});
	m_cameraTransformCB->Update ();
	m_cameraTransformCB->Bind ();
	m_rt->Bind ();
	m_rt->Clear ( 0,0,0);
}

Camera::~Camera() {
	//delete(m_cameraTransformCB);
}

void Camera::Unbind( ) const {
	m_rt->Unbind ();
}

DirectX::XMMATRIX Camera::getProj() const {
	return m_proj;
}

void Camera::Insert(Node* node, const Transform& worldTransform)
{
	GetRenderer()->SubmitCamera(this, &worldTransform);
}

REFLECT_STRUCT_BEGIN(Camera, Entity)
REFLECT_STRUCT_MEMBER(m_fov)
REFLECT_STRUCT_MEMBER(m_aspectratio)
REFLECT_STRUCT_MEMBER(m_near)
REFLECT_STRUCT_MEMBER(m_far)
REFLECT_STRUCT_MEMBER(m_RTId)
REFLECT_STRUCT_MEMBER(m_orthographic)
REFLECT_STRUCT_MEMBER(m_tagMask)
REFLECT_STRUCT_MEMBER(m_priority)
REFLECT_STRUCT_END(Camera)

REGISTER_ENTITY(Camera)
