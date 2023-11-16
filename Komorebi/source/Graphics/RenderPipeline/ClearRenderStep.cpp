#include "Graphics/RenderPipeline/ClearRenderStep.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Engine.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"

namespace gfx {
  void ClearRenderStep::ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const {
    if (m_outRt) {
      m_outRt->Clear(0, 0, 0, 0);
    }
  }
}

typedef gfx::ClearRenderStep ClearRenderStepType;
REFLECT_STRUCT_BEGIN(ClearRenderStepType, gfx::RenderStep)
REFLECT_STRUCT_END(ClearRenderStepType)
