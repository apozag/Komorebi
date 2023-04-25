#pragma once

#include <vector>
#include "Core/Reflection/ReflectionMacros.h"
#include "Core/GameObject.h"

namespace gfx {

  class Material;
  class ResourceBindable;

  class MaterialInstance : public GameObject {
  public:
    MaterialInstance() {}
    MaterialInstance(Material* material) : m_material(material) {}

    void Bind();
    void Unbind();

    void AddBindable(ResourceBindable* bind) { m_binds.push_back(bind); }

    Material* GetMaterial() { return m_material; }
    
    REFLECT_BASE()

  private:    
    Material* m_material;
    std::vector<OWNED_PTR(ResourceBindable)> m_binds;
  };
}

DECLARE_REFLECTION_POINTER(gfx::MaterialInstance)