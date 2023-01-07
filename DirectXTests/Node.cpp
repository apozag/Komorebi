#include "Node.h"

REFLECT_STRUCT_BASE_BEGIN(Node)
REFLECT_STRUCT_MEMBER(m_entities)
REFLECT_STRUCT_MEMBER(m_localTransform)
REFLECT_STRUCT_MEMBER(m_children)
REFLECT_STRUCT_END()

IMPLEMENT_REFLECTION_POINTER(Node*, Node)