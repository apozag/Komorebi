#pragma once

#include "Script.h"

class CameraMovement : public Script {
	void Start(Node* node) override;
	void Update(Node * node) override;
};