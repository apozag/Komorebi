#define _XM_NO_INTRINSICS_

#include "Graphics/BindableSlotsInfo.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Memory/Factory.h"
#include "Entities/Camera.h"
#include "Scene\Scene.h"
#include "Core/Math/Transform.h"
#include "Graphics/Bindables/Resource/ConstantBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Graphics/PipelineStage.h"

void Camera::Setup() {

  Reconfigure();
	m_cameraTransformCB = memory::Factory::Create<gfx::ConstantBuffer<CameraTransformCB>>(
    VCBUFF_CAMERATRANSFORM_SLOT, true, nullptr, (unsigned int)(gfx::PipelineStage::VERTEX | gfx::PipelineStage::GEOMETRY | gfx::PipelineStage::PIXEL));
}

void Camera::Reconfigure() {
  if (m_near > 0 && m_far > 0) {
    if (m_orthographic) {
      if (m_orthoWidth > 0 && m_orthoHeight > 0)
        m_proj = DirectX::XMMatrixOrthographicLH(m_orthoWidth, m_orthoHeight, m_near, m_far);
    }
    else {
      if (m_fov > 0 && m_aspectratio > 0)
        m_proj = DirectX::XMMatrixPerspectiveFovLH(m_fov, m_aspectratio, m_near, m_far);
    }
  }
}

void Camera::Bind( const Transform* worldTransform) const {
	const DirectX::XMMATRIX view = worldTransform->GetInverseMatrixUnsafe();
	const DirectX::XMMATRIX& viewInv = worldTransform->GetMatrix();
	DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, m_proj);
	m_cameraTransformCB->m_buffer = CameraTransformCB{
		DirectX::XMMatrixTranspose(viewProj),
		DirectX::XMMatrixTranspose(view),
		DirectX::XMMatrixTranspose(m_proj),
    viewInv,
    {m_near, m_far, m_orthoWidth, m_orthoHeight}
	};
	m_cameraTransformCB->Update ();
	m_cameraTransformCB->Bind ();

}

Camera::~Camera() {
  memory::Factory::Destroy(m_cameraTransformCB);
}

void Camera::Unbind( ) const {
  m_cameraTransformCB->Unbind();
}

DirectX::XMMATRIX Camera::getProj() const {
	return m_proj;
}

void Camera::Insert(Node* node, const Transform& worldTransform)
{
	GetRenderer()->SubmitCamera(this, &worldTransform);
}

std::vector<DirectX::XMFLOAT4> Camera::GetFrustumPlanes(const Transform& transform) const  {

  DirectX::XMMATRIX viewProj = transform.GetInverseMatrixUnsafe() * getProj();

  // x, y, z, and w represent A, B, C and D in the plane equation
  // where ABC are the xyz of the planes normal, and D is the plane constant
  std::vector<DirectX::XMFLOAT4> tempFrustumPlane(6);

  // Left Frustum Plane
  // Add first column of the matrix to the fourth column
  tempFrustumPlane[0].x = viewProj._14 + viewProj._11;
  tempFrustumPlane[0].y = viewProj._24 + viewProj._21;
  tempFrustumPlane[0].z = viewProj._34 + viewProj._31;
  tempFrustumPlane[0].w = viewProj._44 + viewProj._41;

  // Right Frustum Plane
  // Subtract first column of matrix from the fourth column
  tempFrustumPlane[1].x = viewProj._14 - viewProj._11;
  tempFrustumPlane[1].y = viewProj._24 - viewProj._21;
  tempFrustumPlane[1].z = viewProj._34 - viewProj._31;
  tempFrustumPlane[1].w = viewProj._44 - viewProj._41;

  // Top Frustum Plane
  // Subtract second column of matrix from the fourth column
  tempFrustumPlane[2].x = viewProj._14 - viewProj._12;
  tempFrustumPlane[2].y = viewProj._24 - viewProj._22;
  tempFrustumPlane[2].z = viewProj._34 - viewProj._32;
  tempFrustumPlane[2].w = viewProj._44 - viewProj._42;

  // Bottom Frustum Plane
  // Add second column of the matrix to the fourth column
  tempFrustumPlane[3].x = viewProj._14 + viewProj._12;
  tempFrustumPlane[3].y = viewProj._24 + viewProj._22;
  tempFrustumPlane[3].z = viewProj._34 + viewProj._32;
  tempFrustumPlane[3].w = viewProj._44 + viewProj._42;

  // Near Frustum Plane
  // We could add the third column to the fourth column to get the near plane,
  // but we don't have to do this because the third column IS the near plane
  tempFrustumPlane[4].x = viewProj._13;
  tempFrustumPlane[4].y = viewProj._23;
  tempFrustumPlane[4].z = viewProj._33;
  tempFrustumPlane[4].w = viewProj._43;

  // Far Frustum Plane
  // Subtract third column of matrix from the fourth column
  tempFrustumPlane[5].x = viewProj._14 - viewProj._13;
  tempFrustumPlane[5].y = viewProj._24 - viewProj._23;
  tempFrustumPlane[5].z = viewProj._34 - viewProj._33;
  tempFrustumPlane[5].w = viewProj._44 - viewProj._43;

  // Normalize plane normals (A, B and C (xyz))
  // Also take note that planes face inward
  for (int i = 0; i < 6; ++i) {
    float length = sqrt((tempFrustumPlane[i].x * tempFrustumPlane[i].x) + (tempFrustumPlane[i].y * tempFrustumPlane[i].y) + (tempFrustumPlane[i].z * tempFrustumPlane[i].z));
    tempFrustumPlane[i].x /= length;
    tempFrustumPlane[i].y /= length;
    tempFrustumPlane[i].z /= length;
    tempFrustumPlane[i].w /= length;
  }

  return tempFrustumPlane;
}

REFLECT_STRUCT_BEGIN(Camera, Entity)
REFLECT_STRUCT_MEMBER(m_fov)
REFLECT_STRUCT_MEMBER(m_aspectratio)
REFLECT_STRUCT_MEMBER(m_near)
REFLECT_STRUCT_MEMBER(m_far)
REFLECT_STRUCT_MEMBER(m_orthoWidth)
REFLECT_STRUCT_MEMBER(m_orthoHeight)
REFLECT_STRUCT_MEMBER(m_orthographic)
REFLECT_STRUCT_MEMBER(m_tagMask)
REFLECT_STRUCT_MEMBER(m_priority)
REFLECT_STRUCT_MEMBER(m_renderPipelineId)
REFLECT_STRUCT_END(Camera)

REGISTER_ENTITY(Camera)
