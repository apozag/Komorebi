#include "Graphics\MaterialInstance.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Graphics\Material.h"
#include "Graphics\Bindables\Resource\ResourceBindable.h"

namespace gfx {

  void MaterialInstance::Setup() {

  }

  void MaterialInstance::Bind() {
    //m_material->Bind();
    for (ResourceBindable* bind : m_binds) {
      bind->Bind();
    }
  }

  void MaterialInstance::Unbind() {
    //m_material->Unbind();
    for (ResourceBindable* bind : m_binds) {
      bind->Unbind();
    }
  }

  REFLECT_STRUCT_BASE_BEGIN(MaterialInstance)
  REFLECT_STRUCT_MEMBER(m_material)
  REFLECT_STRUCT_MEMBER(m_binds)
  REFLECT_STRUCT_END(MaterialInstance)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, MaterialInstance)