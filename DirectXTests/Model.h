#pragma once

#include "Drawable.h"

class Mesh;
class SkinnedMesh;
class Skeleton;
class Animation;

class Model : public Entity{
public:
	Model( );
	//void SetMaterial(Material* material);
	void AddPass(Pass* pass);
	void AddDrawable(Drawable* mesh);
	const Drawable::BVHData& GetBVHData() const;
	void Insert(Node* node, const Transform& worldTransform) override;
public:
	Skeleton* m_skeleton;
	Animation* m_animation;
	bool m_hasAnimation = false;
private:
	std::vector<Drawable*> m_drawables;
	Drawable::BVHData m_bvh;
};
