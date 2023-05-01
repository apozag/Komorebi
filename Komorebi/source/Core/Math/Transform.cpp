#include "Core/Math/Transform.h"

#include "Core/Reflection/ReflectionImplMacros.h"

void Transform::Setup() {
	compose();
}

Transform operator*(Transform leftTransform, Transform rightTransform) {
	return Transform(leftTransform.GetMatrix() * rightTransform.GetMatrix());
}

REFLECT_STRUCT_BASE_BEGIN(Transform)
REFLECT_STRUCT_MEMBER(m_position)
REFLECT_STRUCT_MEMBER(m_rotation)
REFLECT_STRUCT_MEMBER(m_scale)
REFLECT_STRUCT_END(Transform)