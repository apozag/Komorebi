#pragma once

#include "Entities/Script.h"

class Rotate : public Script {
	void Start(Node* node) override;
	void Update(Node* node) override;
public:
	REFLECT()
};

DECLARE_REFLECTION_POINTER(Rotate)