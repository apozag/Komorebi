#include "Transform.h"


Transform operator*(Transform leftTransform, Transform rightTransform) {
	return Transform(leftTransform.GetMatrix() * rightTransform.GetMatrix());
}