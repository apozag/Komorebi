#pragma once

#include "Entities/Script.h"

namespace gfx {
	class Material;
}

class ChangeColor : public Script {
	void Run(Node* node) override;
	void Update(Node* node) override;
private:
	gfx::Material* m_material;
	float m_color[4] = { 0,0,0,1 };
};