#pragma once

#include <d3d11.h>
#include "SimpleMath.h"
#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

namespace math {
  class Vector3 : public DirectX::SimpleMath::Vector3, public GameObject {
  public:
    Vector3() {}
    Vector3(DirectX::SimpleMath::Vector3& v) { x = v.x; y = v.y; z = v.z; }
    Vector3(DirectX::SimpleMath::Vector3&& v) { x = v.x; y = v.y; z = v.z; }
    Vector3(float x, float y, float z) : DirectX::SimpleMath::Vector3(x, y, z) {}
    Vector3(std::initializer_list<Vector3> l) {}
    REFLECT_BASE()
    using DirectX::SimpleMath::Vector3::operator=;
  };

  class Quaternion : public DirectX::SimpleMath::Quaternion, public GameObject {
  public:
    Quaternion() {}
    Quaternion(DirectX::SimpleMath::Quaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; }
    Quaternion(DirectX::SimpleMath::Quaternion&& q) { x = q.x; y = q.y; z = q.z; w = q.w; }
    Quaternion(float x, float y, float z, float w) : DirectX::SimpleMath::Quaternion(x, y, z, w) {}
    REFLECT_BASE()
    using DirectX::SimpleMath::Quaternion::operator=;
  };

  class Matrix4 : public DirectX::XMMATRIX, public GameObject {
  public:
    REFLECT_BASE()
    using DirectX::XMMATRIX::operator=;
  };
}