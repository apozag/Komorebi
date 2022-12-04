#pragma once

#include "Script.h"

class CameraMovement : public Script {
	void Run(Node* node) override;
	void Update(Node * node) override;
};