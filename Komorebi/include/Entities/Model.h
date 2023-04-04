#pragma once

#include <string>
#include "Entities/Drawable.h"
#include "Animation/Skeleton.h"
#include "Core\Math\BvhData.h"

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
  ~Model();

  virtual void Setup() override;
  virtual void Reconfigure() override;
  void AddPass(gfx::Pass* pass);
  void AddBindable(gfx::ResourceBindable* bind);
  void AddDrawable(Drawable* mesh);
  const BVHData& GetBVHData() const;
  const std::vector<OWNED_PTR(gfx::Pass)>& GetPasses() const { return m_passes; }
  const std::vector<OWNED_PTR(gfx::ResourceBindable)>& GetBinds() const { return m_binds; }
  void Insert(Node* node, const Transform& worldTransform) override;
  REFLECT()
public:
  Skeleton m_skeleton;
  Animation* m_animation;
  bool m_hasAnimation = false;
private:
  std::vector<Drawable*> m_drawables;
  BVHData m_bvh;

/// Serialized members
private:
  std::string m_filename;
  std::vector<OWNED_PTR(gfx::Pass)> m_passes;
  std::vector<OWNED_PTR(gfx::ResourceBindable)> m_binds;
};
