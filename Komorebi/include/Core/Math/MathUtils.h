#pragma once

#include <vector>
#include "Core/Math/BvhData.h"

class Transform;

namespace math {

  bool cullAABB(const std::vector<DirectX::XMFLOAT4>& frustumPlanes, const BVHData& bvhData, const Transform* worldTransform);

  void UpdateAABB(const BVHData& a, const Transform& transform, BVHData& b);

}
