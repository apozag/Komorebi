#pragma once

#include <string>
#include "Entities/Drawable.h"
#include "Animation/Skeleton.h"

class Mesh;
class SkinnedMesh;
class Animation;
class ResourceBindable;

class Model : public Entity {
public:
  Model() {}
  Model(const char* filename) 
    : m_filename(filename)
  {}

  virtual void Setup() override;
  void AddPass(Pass* pass);
  void AddBindable(ResourceBindable* bind);
  void AddDrawable(Drawable* mesh);
  const Drawable::BVHData& GetBVHData() const;
  const std::vector<OWNED_PTR(Pass)>& GetPasses() const { return m_passes; }
  const std::vector<OWNED_PTR(ResourceBindable)>& GetBinds() const { return m_binds; }
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
  std::vector<OWNED_PTR(Pass)> m_passes;
  std::vector<OWNED_PTR(ResourceBindable)> m_binds;
};
