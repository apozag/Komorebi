#include "BindableSlotsInfo.h"

#include "Camera.h"
#include "Transform.h"
#include "ConstantBuffer.h"
#include "Renderer.h"
#include "RenderTarget.h"

Camera::Camera( float fov, float aspectratio, float nearZ, float farZ, RenderTarget* rt, bool orthographic) : m_near(nearZ), m_far(farZ), m_rt(rt){
	if (orthographic) {
		m_proj = DirectX::XMMatrixOrthographicLH(100, 100, nearZ, farZ);
	}
	else {
		m_proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspectratio, nearZ, 1000);
	}
	m_cameraTransformCB = new VertexConstantBuffer<CameraTransformCB> ( VCBUFF_CAMERATRANSFORM_SLOT);
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
