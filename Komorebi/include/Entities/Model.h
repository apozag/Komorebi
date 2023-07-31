#pragma once

#include <string>
#include "Entities/Drawable.h"
#include "Animation/Skeleton.h"
#include "Core\Math\BvhData.h"

class Mesh;
class SkinnedMesh;
class Animation;
class ResourceBindable;

namespace gfx {
  class Material;
}

class Model : public Entity {
public:
  Model() {}
  Model(const char* filename) 
    : m_filename(filename)
  {}
  ~Model();

  virtual void Setup() override;
  virtual void Reconfigure() override;
  void AddDrawable(Drawable* mesh);
  const BVHData& GetBVHData() const;
  gfx::Material* GetMaterial() const { return m_material; }
  void Insert(Node* node, const Transform& worldTransform) override;
  REFLECT()
public:
  Skeleton m_skeleton;
  Animation* m_animation = nullptr;
  bool m_hasAnimation = false;
private:
  std::vector<Drawable*> m_drawables;
  BVHData m_bvh;

/// Serialized members
private:
  std::string m_filename;  
  ASSET_PTR(gfx::Material) m_material;
};
