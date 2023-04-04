#include <algorithm>

#include "Graphics\RenderPipeline.h"
#include "Graphics\RenderInfo.h"
#include "Core\Engine.h"
#include "Core/Math/Transform.h"
#include "Core/Math/MathUtils.h"
#include "Graphics\Renderer.h"
#include "Entities\Camera.h"
#include "Entities\Drawable.h"
#include "Graphics\Pass.h"
#include "Graphics\Material.h"
#include "Graphics\Bindables\Resource\RenderTarget.h"
#include "Graphics\Bindables\Resource\Texture2D.h"

bool compareJob(const gfx::Job& j1, const gfx::Job& j2) {
  return j1.key < j2.key;
}

namespace gfx { 

  void RenderPipeline::Setup() {

    const RenderInfo* renderInfo = Engine::GetRenderer()->GetRenderInfo();
    for (RenderStep& step : m_steps) {
      for (RenderStep::TextureInfo& texInfo : step.m_inputsInfo){
        const RenderTarget* rt = renderInfo->FindGlobalRenderTarget(texInfo.m_rtId);
        if (rt != nullptr) {
          // TODO: [ERROR] Texture index is higher than texture count
          step.m_inRts.push_back(rt->GetTextures2D()[texInfo.m_textureIdx]);
        }
      }

      if (step.m_outRtId == "DEFAULT") {
        step.m_outRt = Engine::GetDefaultRendertarget();
      }
      else {
        step.m_outRt = renderInfo->FindGlobalRenderTarget(step.m_outRtId);
      }
    }
  }

  void RenderPipeline::Execute(CameraView camView, std::vector<Job>& jobs) const {

    unsigned int jobsToExecute = jobs.size();

    // Get frustum planes for culling
    const std::vector<DirectX::XMFLOAT4>& planes = camView.camera->GetFrustumPlanes(*camView.transform);

    // Update job sorting keys for this camera. 
    for (int i = 0; i < jobs.size(); i++) {

      Job& job = jobs[i];

      const RenderStep* step = FindRenderStep(job);

      // Frustum culling
      bool ignoreCull = job.pass->DoesIgnoreFrustumCulling();
      bool cull =  !ignoreCull && math::cullAABB(planes, job.drawable->GetBVHData(), job.transform);

      job.key &= ~(uint64_t(1) << 34);
      job.key |= uint64_t(cull) << 34;

      jobsToExecute -= cull;

      // Depth
      if (!cull) {
        unsigned int shifts = step->m_sortReverse * 16;
        job.key &= ~(uint64_t(0xFFFF) << shifts);
        float depth = camView.transform->PointToLocalUnsafe(job.transform->GetPositionUnsafe()).Length();
        float farZ = camView.camera->m_far;
        float nearZ = camView.camera->m_near;
        float normalizedDepth = (depth - nearZ) / (farZ - nearZ);
        uint64_t depthKeyComponent = (step->m_sortReverse ? 1.0f - normalizedDepth : normalizedDepth) * 0xFFFF;
        job.key |= depthKeyComponent << shifts;
        job.key |= 0;
      }

    }

    // Sort jobs based on its key
    std::sort(jobs.begin(), jobs.end(), compareJob);

    // Bind camera
    camView.camera->Bind(camView.transform); 

    // Execute render steps
    unsigned int startJobIdx = 0u;
    for (const RenderStep& step : m_steps) {
      step.Execute(jobs, jobsToExecute, startJobIdx);
      startJobIdx = step.m_maxLayer;
    }

    /*std::ostringstream os_;
    os_ << "setPass: " << stateBindCount <<
      " setMat: " << resourceBindCount <<
      " DrawCalls: " << jobsToExecute << "\n";
    OutputDebugString(os_.str().c_str());*/

    camView.camera->Unbind();
  }

  const RenderStep* RenderPipeline::GetRenderStep(unsigned int idx) const {
    return &m_steps[idx];
  }

  const RenderStep* RenderPipeline::FindRenderStep(const Job& job) const { 
    unsigned int layer = job.pass->m_layer;
    for (const RenderStep& step : m_steps) {
      if (layer <= step.m_maxLayer) {
        return &step;
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