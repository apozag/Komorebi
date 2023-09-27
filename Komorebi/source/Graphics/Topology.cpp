#include "Graphics/Topology.h"

#include "Core/Reflection/ReflectionImplMacros.h"

typedef gfx::Topology GeomTopology;
REFLECT_ENUM_BEGIN(GeomTopology)
REFLECT_ENUM_VALUE(TRIANGLESTRIP)
REFLECT_ENUM_VALUE(TRIANGLES)
REFLECT_ENUM_VALUE(POINTS)
REFLECT_ENUM_VALUE(LINES)
REFLECT_ENUM_END(GeomTopology)