#pragma once

#include "Entities/Script.h"
#include "SimpleMath.h"

class CameraMovement : public Script {
public:
	REFLECT()
private:

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;	

	float m_speed = 100.f;
	float m_sensitivity = 0.3f;

	void Start(Node* node) override;
	void Update(Node * node) override;
};