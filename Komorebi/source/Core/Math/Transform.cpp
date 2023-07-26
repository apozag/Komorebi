#include "Core/Math/Transform.h"

#include "Core/Reflection/ReflectionImplMacros.h"

const Transform* Transform::s_identity;

void Transform::Setup() {
	compose();
}

const Transform& Transform::GetIdentity() {
	if (!s_identity) {
		s_identity = memory::Factory::Create<Transform>();
	}
	return *s_identity;
}

Transform operator*(Transform leftTransform, Transform rightTransform) {
	return Transform(leftTransform.GetMatrix() * rightTransform.GetMatrix());
}

REFLECT_STRUCT_BASE_BEGIN(Transform)
REFLECT_STRUCT_MEMBER(m_position)
REFLECT_STRUCT_MEMBER(m_rotation)
REFLECT_STRUCT_MEMBER(m_scale)
REFLECT_STRUCT_END(Transform)