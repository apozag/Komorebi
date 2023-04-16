#pragma once

#include "Entities/Script.h"

class CameraMovement : public Script {
public:
	REFLECT()
private:
	float m_speed = 100.f;
	float m_sensitivity = 0.3f;
	void Run(Node* node) override;
	void Update(Node * node) override;
};