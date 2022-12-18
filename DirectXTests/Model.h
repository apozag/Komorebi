#pragma once

#include <string>
#include "Drawable.h"
#include "Skeleton.h"

class Mesh;
class SkinnedMesh;
class Animation;

class Model : public Entity {
public:
  Model();
  Model(const char* filename, Scene* scene, Node* parent = nullptr);
  //void SetMaterial(Material* material);
  void AddPass(Pass* pass);
  void AddDrawable(Drawable* mesh);
  const Drawable::BVHData& GetBVHData() const;
  void Insert(Node* node, const Transform& worldTransform) override;
  REFLECT()
public:
  Skeleton m_skeleton;
  Animation* m_animation;
  bool m_hasAnimation = false;
private:
  std::vector<Drawable*> m_drawables;
  Drawable::BVHData m_bvh;

/// Serialized members
private:
  std::string m_filename;
};
