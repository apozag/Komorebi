#pragma once

#include "Script.h"

class Material;

class ChangeColor : public Script {
	void Start(Node* node) override;
	void Update(Node* node) override;
private:
	Material* m_material;
	float m_color[4] = { 0,0,0,1 };
};