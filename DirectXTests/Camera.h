#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Graphics.h"
#include "SimpleMath.h"
#include "Entity.h"
#include "ConstantBuffer.h"

using namespace DirectX::SimpleMath;

class Camera : public Entity{
public:
	struct CameraTransformCB {
		DirectX::XMMATRIX viewProj;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};
public:
	Camera(Graphics& gfx, float fov, float aspectratio, float nearZ, float farZ, RenderTarget* rt, bool orthographic = false);
	void Bind(Graphics& gfx, const Transform* worldTransform) const;
	void Unbind(Graphics& gfx) const;
	DirectX::XMMATRIX getProj() const;

	float m_near, m_far;

	DirectX::XMMATRIX m_proj;

	VertexConstantBuffer<Camera::CameraTransformCB>* m_cameraTransformCB;
	RenderTarget* m_rt;

	virtual void Insert(Renderer& renderer, const Transform& worldTransform) override;
};
