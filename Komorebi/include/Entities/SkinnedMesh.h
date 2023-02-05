#pragma once

#include <vector>

#include <DirectXMath.h>
#include <d3d11.h>

#include "Entities/Drawable.h"
#include "Core/Defines.h"

class Skeleton;

class SkinnedMesh : public Drawable {
public:
	SkinnedMesh( std::vector<POD::SkinnedVertex>& vertices, std::vector<unsigned short>& indices, Skeleton* skeleton, BVHData bvhData = BVHData());
	SkinnedMesh(const SkinnedMesh& drawable);
	SkinnedMesh* Clone();
	void Draw(const DirectX::XMMATRIX& modelMatrix) const override;
private:
	Skeleton* m_skeleton;
};