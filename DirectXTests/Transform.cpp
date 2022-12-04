#include "Transform.h"

Transform operator*(Transform leftTransform, Transform rightTransform) {
	return Transform(leftTransform.GetMatrix() * rightTransform.GetMatrix());
}

std::ostream& operator<<(std::ostream& os, Transform transform) {
	os << "[transform_data]";
	return os;
}

IMPLEMENT_REFLECTION_PRIMITIVE(Transform, Transform)