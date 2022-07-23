#pragma once

#include "Drawable.h"

class Mesh;
class SkinnedMesh;
class Skeleton;

class Model : public Entity{
public:
	Model(Graphics& gfx);
	void AddPass(Pass* pass);
	void AddDrawable(Drawable* mesh);
	const Drawable::BVHData& GetBVHData() const;
	void Insert(Renderer& renderer, const Transform& worldTransform) override {}
public:
	Skeleton* m_skeleton;
private:
	std::vector<Drawable*> m_drawables;
	Drawable::BVHData m_bvh;
};
