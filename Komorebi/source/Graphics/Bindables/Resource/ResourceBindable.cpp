#include "Graphics/Bindables/Resource/ResourceBindable.h"
namespace gfx {
  REFLECT_STRUCT_VIRTUAL_BEGIN(ResourceBindable, Bindable)
  REFLECT_STRUCT_END(ResourceBindable)
}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, ResourceBindable)
IMPLEMENT_REFLECTION_VECTOR(gfx::ResourceBindable*)
IMPLEMENT_REFLECTION_VECTOR(OWNED_PTR(gfx::ResourceBindable))