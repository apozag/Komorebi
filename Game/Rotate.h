#pragma once

#include "Script.h"

class Rotate : public Script {
	void Run(Node* node) override;
	void Update(Node* node) override;
};