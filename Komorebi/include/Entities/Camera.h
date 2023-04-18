	#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include "SimpleMath.h"
#include "Entities/Entity.h"
#include "Core/Reflection/ReflectionMacros.h"

#define MAIN_CAMERA_TAG 5

namespace gfx {
	template<typename T>
	class VertexConstantBuffer;
	class RenderTarget;
}

class Camera : public Entity{
public:
	struct CameraTransformCB {
		DirectX::XMMATRIX viewProj;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};
public:
	Camera(float fov, float aspectratio, float nearZ, float farZ, bool orthographic = false, float orthoWidth = 500, float orthoHeight = 500)
		: m_fov(fov), m_aspectratio(aspectratio), m_near(nearZ), m_far(farZ), m_priority(0), m_orthographic(orthographic), m_orthoWidth(orthoWidth), m_orthoHeight(orthoHeight)
	{}
	Camera() = default;
	~Camera();

	void Setup() override;
	void Reconfigure() override;
	void Bind(const Transform* worldTransform) const;
	void Unbind( ) const;
	DirectX::XMMATRIX getProj() const;
	void Insert(Node* node, const Transform& worldTransform) override;

	float GetNear() const { 
		float nearZ = m_near;
		return nearZ; 
	}
	float GetFar() const {
		float farZ = m_far;
		return farZ; 
	}
	float GetFov() const { return m_fov; }
	float GetAspect() const { return m_aspectratio; }
	bool IsOrthographic() const { return m_orthographic; }

	std::vector<DirectX::XMFLOAT4> GetFrustumPlanes(const Transform& transform) const;

	REFLECT()

public:

	float m_fov, m_aspectratio, m_near, m_far, m_orthoWidth, m_orthoHeight;
	bool m_orthographic = false;
	std::string m_renderPipelineId;

public:
	uint32_t m_tagMask = 0xFFFFFFFF;
	int m_priority;

	DirectX::XMMATRIX m_proj;
	gfx::VertexConstantBuffer<Camera::CameraTransformCB>* m_cameraTransformCB;

	

};
