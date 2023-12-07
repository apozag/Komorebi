#include <algorithm>
//#include <sstream>

#include "Graphics/RenderPipeline.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Graphics/RenderInfo.h"
#include "Core/Engine.h"
#include "Core/Math/Transform.h"
#include "Core/Math/MathUtils.h"
#include "Graphics/Renderer.h"
#include "Entities/Camera.h"
#include "Entities/Drawable.h"
#include "Graphics/RenderPipeline/GeometryRenderStep.h"
#include "Graphics/Pass.h"
#include "Graphics/Material.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Graphics/Bindables/Resource/Texture2D.h"

bool compareJob(const gfx::Job& j1, const gfx::Job& j2) {
  return j1.key < j2.key;
}

namespace gfx {

  void RenderPipeline::Setup() {
  }

  void RenderPipeline::Execute(const CameraView& camView, std::vector<Job>& jobs) const {

    unsigned int jobsToExecute = jobs.size();

    // Get frustum planes for culling
    const std::vector<DirectX::XMFLOAT4>& planes = camView.camera->GetFrustumPlanes(*camView.transform);

    // Build job sorting keys
    //Opaque:			    15 empty + 1 culling + 16 layer + 8 matIdx + 8 passIdx + 16 depth
    //Transparent:		15 empty + 1 culling + 16 layer + 16 depth + 8 matIdx + 8 passIdx
    for (int i = 0; i < jobs.size(); i++) {

      Job& job = jobs[i];

      const GeometryRenderStep* step = FindGeomRenderStep(job);

      // Frustum culling
      bool ignoreCull = job.pass->DoesIgnoreFrustumCulling();
      bool cull = step == nullptr || !ignoreCull && math::cullAABB(planes, job.drawable->GetBVHData(), job.transform);      

      job.key = 0u;
      ////job.key &= ~(uint64_t(1) << 48);
      job.key |= uint64_t(cull) << 48;

      jobsToExecute -= cull;

      if (!cull) {

        unsigned isTransparent = (unsigned)step->GetSortReverse();
        unsigned isOpaque = 1u - isTransparent;

        // Pass layer    
        unsigned int layer = job.pass->m_layer > 0xFFFF ? 0xFFFF : job.pass->m_layer;
        job.key |= uint64_t(layer) << 32u;

        // Material (resource binds)
        unsigned int idx = job.material->GetMaterial()->GetIdx();
        unsigned int shifts = 8u + 16u * isOpaque;
        job.key |= ((uint64_t)idx) << shifts;

        // Pass index (state binds)
        shifts = 16u * isOpaque;
        job.key |= uint64_t(job.pass->GetIdx()) << shifts;

        // Depth
        shifts = isTransparent * 16;
        job.key &= ~(uint64_t(0xFFFF) << shifts);
        float depth = camView.transform->PointToLocalUnsafe(job.transform->GetPositionUnsafe()).Length();
        float farZ = camView.camera->m_far;
        float nearZ = camView.camera->m_near;
        float normalizedDepth = (depth - nearZ) / (farZ - nearZ);
        uint64_t depthKeyComponent = (isTransparent ? 1.0f - normalizedDepth : normalizedDepth) * 0xFFFF;
        job.key |= depthKeyComponent << shifts;
      }
    }

    // Sort jobs based on its key
    std::sort(jobs.begin(), jobs.end(), compareJob);

    // Bind camera
    camView.camera->Bind(camView.transform);

    // Execute render steps
    unsigned int startJobIdx = 0u;
    unsigned int endJobIdx = 0u;
    for (const RenderStep* step : m_steps) {
      step->Execute(jobs, jobsToExecute, startJobIdx, endJobIdx);
      startJobIdx = endJobIdx;
    }

    /*std::ostringstream os_;
    os_ << " Job to execute: " << jobsToExecute << "\n";
    OutputDebugString(os_.str().c_str());*/

    camView.camera->Unbind();
  }

  const RenderStep* RenderPipeline::GetRenderStep(unsigned int idx) const {
    return m_steps[idx];
  }

  const GeometryRenderStep* RenderPipeline::FindGeomRenderStep(const Job& job) const {
    unsigned int layer = job.pass->m_layer;
    for (const RenderStep* step : m_steps) {
      if (step->GetReflectionDynamic() == reflection::TypeResolver<GeometryRenderStep>::get()) {
        const GeometryRenderStep* geomStep = static_cast<const GeometryRenderStep*>(step);
        if (layer <= geomStep->GetMaxLayer()) {
          return geomStep;
        }
      }
    }
    // TODO: [ERROR] No RenderStep found for pass layer
    return nullptr;
  }

  REFLECT_STRUCT_BASE_BEGIN(RenderPipeline)
    REFLECT_STRUCT_MEMBER(m_name)
    REFLECT_STRUCT_MEMBER(m_steps)
    REFLECT_STRUCT_END(RenderPipeline)

}