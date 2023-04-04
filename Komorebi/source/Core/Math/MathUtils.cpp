#define _XM_NO_INTRINSICS_

#include <d3d11.h>
#include "directxmath.h"
#include "Core\Math\MathUtils.h"
#include "Core/Math/Transform.h"

namespace math {

  bool cullAABB(const std::vector<DirectX::XMFLOAT4>& frustumPlanes, const BVHData& bvhData, const Transform* worldTransform) {

    BVHData updatedBvh;
    UpdateAABB(bvhData, *worldTransform, updatedBvh);
    for (int planeID = 0; planeID < 6; ++planeID) {
      DirectX::XMVECTOR planeNormal{ frustumPlanes[planeID].x, frustumPlanes[planeID].y, frustumPlanes[planeID].z, 0.0f };
      float planeConstant = frustumPlanes[planeID].w;

      DirectX::SimpleMath::Vector3 worldMin = updatedBvh.m_min + worldTransform->GetPositionUnsafe();
      DirectX::SimpleMath::Vector3 worldMax = updatedBvh.m_max + worldTransform->GetPositionUnsafe();

      // Check each axis (x,y,z) to get the AABB vertex furthest away from the direction the plane is facing (plane normal)
      DirectX::XMFLOAT3 axisVert;

      // x-axis
      if (frustumPlanes[planeID].x < 0.0f)    // Which AABB vertex is furthest down (plane normals direction) the x axis
        axisVert.x = worldMin.x; // min x plus tree positions x
      else
        axisVert.x = worldMax.x; // max x plus tree positions x

      // y-axis
      if (frustumPlanes[planeID].y < 0.0f)    // Which AABB vertex is furthest down (plane normals direction) the y axis
        axisVert.y = worldMin.y; // min y plus tree positions y
      else
        axisVert.y = worldMax.y; // max y plus tree positions y

      // z-axis
      if (frustumPlanes[planeID].z < 0.0f)    // Which AABB vertex is furthest down (plane normals direction) the z axis
        axisVert.z = worldMin.z; // min z plus tree positions z
      else
        axisVert.z = worldMax.z; // max z plus tree positions z

      // Now we get the signed distance from the AABB vertex that's furthest down the frustum planes normal,
      // and if the signed distance is negative, then the entire bounding box is behind the frustum plane, which means
      // that it should be culled
      if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(planeNormal, XMLoadFloat3(&axisVert))) + planeConstant < 0.0f) {
        return true;
      }
    }
    return false;
  }

  // Transform AABB a by the matrix m and translation t,
// find maximum extents, and store result into AABB b.
  void UpdateAABB(const BVHData& a, const Transform& transform, BVHData& b) {
    const float* amin = (const float*)&a.m_min.x;
    const float* amax = (const float*)&a.m_max.x;
    float* bmin = (float*)&b.m_min.x;
    float* bmax = (float*)&b.m_max.x;
    float* t = (float*)&transform.GetPositionUnsafe();
    const DirectX::XMMATRIX& m = transform.GetMatrix();
    // For all three axes
    for (int i = 0; i < 3; i++) {
      // Start by adding in translation
      bmin[i] = bmax[i] = t[i];
      // Form extent by summing smaller and larger terms respectively
      for (int j = 0; j < 3; j++) {
        float e = m(j, i) * amin[j];
        float f = m(j, i) * amax[j];
        if (e < f) {
          bmin[i] += e;
          bmax[i] += f;
        }
        else {
          bmin[i] += f;
          bmax[i] += e;
        }
      }
    }
  }

}