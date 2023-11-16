#include "Graphics/RenderPipeline/GeometryRenderStep.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Graphics/Pass.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Entities/Drawable.h"
#include "Core/Math/Transform.h"

namespace gfx {

  void GeometryRenderStep::ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const {
    const Pass* lastPass = nullptr;
    const Material* lastMat = nullptr;
    unsigned int i;
    for (i = startIdx; i < jobsToExecute; i++) {

      Job job = jobs[i];

      if (job.pass->m_layer > m_maxLayer) {
        break;
      }

      const Pass* pass = job.pass;
      if (pass != lastPass) {
        if (lastPass) lastPass->Unbind();
        pass->Bind();
        lastPass = pass;
        //stateBindCount++;
      }

      const Material* material = job.material->GetMaterial();
      if (material != lastMat) {
        if (lastMat) lastMat->Unbind();
        material->Bind();
        lastMat = material;
        //resourceBindCount++;
      }

      job.material->Bind();

      job.drawable->Draw(DirectX::XMMatrixTranspose(job.transform->GetMatrix()));

      job.material->Unbind();

      //lastPass = job.pass;
      //lastMat = job.material->GetMaterial();
    }
    endIdx = i;
    if (lastPass) lastPass->Unbind();
    if (lastMat) lastMat->Unbind();
  }
}

typedef gfx::GeometryRenderStep GeometryRenderStepType;
REFLECT_STRUCT_BEGIN(GeometryRenderStepType, gfx::RenderStep)
REFLECT_STRUCT_MEMBER(m_maxLayer)
REFLECT_STRUCT_MEMBER(m_sortReverse)
REFLECT_STRUCT_END(GeometryRenderStepType)
