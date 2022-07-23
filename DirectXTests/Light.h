#pragma once

#include "Entity.h"
#include "Camera.h"
#include "RenderTarget.h"

class Transform;
class Graphics;

class DirectionalLight : public Entity {
public:
	DirectionalLight(Graphics& gfx, DirectX::SimpleMath::Vector3 color);
	void Insert(Renderer& renderer, const Transform& worldTransform) override;
	DirectX::SimpleMath::Vector3 GetColor() const { return color; }
	const Camera* GetCamera() const { return m_camera; }
	RenderTarget* GetShadowMap() { return m_rt; }
private:
	DirectX::SimpleMath::Vector3 color;
	Camera* m_camera;
	RenderTarget* m_rt;
};

class PointLight : public Entity {
public:
	PointLight(Graphics& gfx, DirectX::SimpleMath::Vector3 color);
	void Insert(Renderer& renderer, const Transform& worldTransform) override;
	DirectX::SimpleMath::Vector3 GetColor() const { return color; }
private:
	DirectX::SimpleMath::Vector3 color;
	Camera m_cameras[6];
	RenderTarget m_rts[6];
};

class SpotLight : public Entity {
public:
	SpotLight(Graphics& gfx, DirectX::SimpleMath::Vector3 color, float phi, float theta);
	void Insert(Renderer& renderer, const Transform& worldTransform) override;
	DirectX::SimpleMath::Vector3 GetColor() const { return color; }
private:
	DirectX::SimpleMath::Vector3 color;
	float phi, theta;
	Camera m_camera;
	RenderTarget m_rt;
};
