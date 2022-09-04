#pragma once

#include "Entity.h"
#include "Camera.h"
#include "RenderTarget.h"

class Transform;
class Graphics;

class DirectionalLight : public Entity {
public:
	DirectionalLight( DirectX::SimpleMath::Vector3 color);
	void Insert(Node* node, const Transform& worldTransform) override;
	DirectX::SimpleMath::Vector3 GetColor() const { return color; }
	const Camera* GetCamera() const { return m_camera; }
	Texture2D* GetShadowMap() const { return m_rt->GetTextures2D()[0]; }
private:
	DirectX::SimpleMath::Vector3 color;
	Camera* m_camera;
	RenderTarget* m_rt;
};

class PointLight : public Entity {
public:
	PointLight( DirectX::SimpleMath::Vector3 color);
	void Insert(Node* node, const Transform& worldTransform) override;
	DirectX::SimpleMath::Vector3 GetColor() const { return color; }
private:
	DirectX::SimpleMath::Vector3 color;
	Camera m_cameras[6];
	RenderTarget m_rts[6];
};

class SpotLight : public Entity {
public:
	SpotLight( DirectX::SimpleMath::Vector3 color, float phi, float theta);
	void Insert(Node* node, const Transform& worldTransform) override;
	DirectX::SimpleMath::Vector3 GetColor() const { return color; }
private:
	DirectX::SimpleMath::Vector3 color;
	float phi, theta;
	Camera m_camera;
	RenderTarget m_rt;
};
