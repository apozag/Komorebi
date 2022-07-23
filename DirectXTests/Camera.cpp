#include "Camera.h"
#include "Transform.h"
#include "ConstantBuffer.h"
#include "Renderer.h"
#include "RenderTarget.h"

Camera::Camera(Graphics& gfx, float fov, float aspectratio, float nearZ, float farZ, RenderTarget* rt, bool orthographic) : m_near(nearZ), m_far(farZ), m_rt(rt){
	if (orthographic) {
		m_proj = DirectX::XMMatrixOrthographicLH(20, 20, nearZ, farZ);
	}
	else {
		m_proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspectratio, nearZ, farZ);
	}
	m_cameraTransformCB = new VertexConstantBuffer<CameraTransformCB>(gfx, VIEW_CBUFF_SLOT);
	//m_id = static_id++;
}

void Camera::Bind(Graphics& gfx, const Transform* worldTransform) const {
	DirectX::XMMATRIX view = worldTransform->GetInverseMatrixUnsafe();
	DirectX::XMMATRIX proj = m_proj;
	DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	m_cameraTransformCB->SetBuffer(CameraTransformCB{
		DirectX::XMMatrixTranspose(viewProj),
		DirectX::XMMatrixTranspose(view),
		DirectX::XMMatrixTranspose(m_proj)
		});
	m_cameraTransformCB->Update(gfx);
	m_cameraTransformCB->Bind(gfx);
	m_rt->SetAsRenderTarget();	
	m_rt->Bind(gfx);
	m_rt->Clear(gfx, 0,0,0);
	m_rt->Lock();
}

void Camera::Unbind(Graphics& gfx) const {
	m_rt->Unlock();
	m_rt->SetAsShaderResource();
}

DirectX::XMMATRIX Camera::getProj() const {
	return m_proj;
}

void Camera::Insert(Renderer& renderer, const Transform& worldTransform)
{
	renderer.SubmitCamera(this, &worldTransform);
}
