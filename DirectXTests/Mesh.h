#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

#include "Drawable.h"
#include "Defines.h"

class Mesh : public Drawable{
public:
	Mesh(Graphics& gfx, std::vector<POD::Vertex>& vertices, std::vector<unsigned short>& indices, BVHData bvhData = BVHData());
	Mesh(const Mesh& drawable);
	Mesh* Clone();
};