#pragma once

#include "Entities/Script.h"

class Rotate : public Script {
	void Run(Node* node) override;
	void Update(Node* node) override;
};