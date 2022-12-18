#include "Transform.h"

Transform operator*(Transform leftTransform, Transform rightTransform) {
	return Transform(leftTransform.GetMatrix() * rightTransform.GetMatrix());
}

std::ostream& operator<<(std::ostream& os, Transform transform) {
	os << "[transform_data]";
	return os;
}

REFLECT_STRUCT_BASE_BEGIN(Transform)
REFLECT_STRUCT_MEMBER(m_position)
REFLECT_STRUCT_MEMBER(m_rotation)
REFLECT_STRUCT_MEMBER(m_scale)
REFLECT_STRUCT_END()