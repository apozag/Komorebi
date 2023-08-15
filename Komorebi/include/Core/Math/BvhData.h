#pragma once

#include "Core/Math/MathWrappers.h"

struct BVHData : public GameObject {

	BVHData() {}

	BVHData(const math::Vector3& min, const math::Vector3& max) : m_min(min), m_max(max) {}

	math::Vector3 m_min;
	math::Vector3 m_max;

	REFLECT_BASE()
};
