#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include "SimpleMath.h"
#include "Entity.h"
#include "ReflectionMacros.h"

template<typename T>
class VertexConstantBuffer;
class PostProcMaterial;

class Camera : public Entity{
public:
	struct CameraTransformCB {
		DirectX::XMMATRIX viewProj;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};
public:
	Camera(float fov, float aspectratio, float nearZ, float farZ, unsigned int rtId, bool orthographic = false)
		: m_near(nearZ), m_far(farZ), m_RTId(rtId), m_priority(0)
	{}
	Camera(float fov, float aspectratio, float nearZ, float farZ, RenderTarget* rt, bool orthographic = false)
		: m_near(nearZ), m_far(farZ), m_rt(rt), m_priority(0)
	{}
	Camera() {};
	void Setup() override;
	void Bind(const Transform* worldTransform) const;
	void Unbind( ) const;
	DirectX::XMMATRIX getProj() const;
	void Insert(Node* node, const Transform& worldTransform) override;

	float GetNear() const { return m_near; }
	float GetFar() const { return m_far; }
	float GetFov() const { return m_fov; }
	float GetAspect() const { return m_aspectratio; }
	bool IsOrthographic() const { return m_orthographic; }

	REFLECT()

public:
	DirectX::XMMATRIX m_proj;
	VertexConstantBuffer<Camera::CameraTransformCB>* m_cameraTransformCB;
	RenderTarget* m_rt;

	//std::vector<PostProcMaterial*> m_postProcMaterials;

	/////////////////////////////////////////////////////
	// Serializable
	/////////////////////////////////////////////////////
private:
	float m_fov, m_aspectratio, m_near, m_far;
	unsigned int m_RTId;
	bool m_orthographic = false;
public:
	uint32_t m_tagMask = 0xFFFFFFFF;
	int m_priority;
};
