#pragma once

#include "Entities/Script.h"

class CameraMovement : public Script {
public:
	REFLECT()
private:
	void Start(Node* node) override;
	void Update(Node * node) override;
};