#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include "SimpleMath.h"
#include "Entity.h"

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
	Camera(float fov, float aspectratio, float nearZ, float farZ, RenderTarget* rt, bool orthographic = false);
	void Bind(const Transform* worldTransform) const;
	void Unbind( ) const;
	DirectX::XMMATRIX getProj() const;
	void Insert(Node* node, const Transform& worldTransform) override;

public:
	int m_priority;
	uint32_t m_tagMask = 0xFFFFFFFF;
	const float m_near, m_far;
	DirectX::XMMATRIX m_proj;
	VertexConstantBuffer<Camera::CameraTransformCB>* m_cameraTransformCB;
	RenderTarget* m_rt;

	std::vector<PostProcMaterial*> m_postProcMaterials;
};
