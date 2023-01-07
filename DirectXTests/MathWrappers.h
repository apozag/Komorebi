#pragma once

#include <d3d11.h>
#include "SimpleMath.h"
#include "ReflectionMacros.h"

namespace math {
  class Vector3 : public DirectX::SimpleMath::Vector3 {
  public:
    Vector3() {}
    Vector3(DirectX::SimpleMath::Vector3& v) { x = v.x; y = v.y; z = v.z; }
    Vector3(DirectX::SimpleMath::Vector3&& v) { x = v.x; y = v.y; z = v.z; }
    REFLECT_BASE()
    using DirectX::SimpleMath::Vector3::operator=;
  };

  class Quaternion : public DirectX::SimpleMath::Quaternion {
  public:
    Quaternion() {}
    Quaternion(DirectX::SimpleMath::Quaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; }
    Quaternion(DirectX::SimpleMath::Quaternion&& q) { x = q.x; y = q.y; z = q.z; w = q.w; }
    REFLECT_BASE()
    using DirectX::SimpleMath::Quaternion::operator=;
  };

  class Matrix4 : public DirectX::XMMATRIX {
  public:
    REFLECT_BASE()
    using DirectX::XMMATRIX::operator=;
  };
}