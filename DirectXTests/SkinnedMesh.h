#pragma once

#include <vector>

#include <DirectXMath.h>
#include <d3d11.h>

#include "Drawable.h"
#include "Defines.h"

class Skeleton;

class SkinnedMesh : public Drawable {
public:
	SkinnedMesh(Graphics& gfx, std::vector<POD::SkinnedVertex>& vertices, std::vector<unsigned short>& indices, Skeleton* skeleton, BVHData bvhData = BVHData());
	SkinnedMesh(const SkinnedMesh& drawable);
	SkinnedMesh* Clone();
	void Draw(Graphics& gfx) const override;
private:
	Skeleton* m_skeleton;
};