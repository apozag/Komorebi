#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

#include "Drawable.h"
#include "Defines.h"

class Mesh : public Drawable {
public:
    Mesh() {}
    Mesh(std::vector<POD::Vertex>& vertices, std::vector<unsigned short>& indices, BVHData bvhData = { {},{} });
    Mesh(const Mesh& drawable);
    Mesh* Clone();

    REFLECT()
};