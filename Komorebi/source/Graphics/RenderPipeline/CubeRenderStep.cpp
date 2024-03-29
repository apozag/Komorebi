#include "Graphics/RenderPipeline/CubeRenderStep.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Engine.h"
#include "Graphics/Pass.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Entities/Drawable.h"

namespace gfx {

  void CubeRenderStep::ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const {
    if (m_material) {
      m_material->Bind();
      for (Pass* pass : m_material->GetPasses()) {
        pass->Bind();
        Engine::GetRenderer()->GetCubePrimitive()->Draw(DirectX::XMMatrixIdentity());
        pass->Unbind();
      }
      m_material->Unbind();
    }
  }
}

typedef gfx::CubeRenderStep CubeRenderStepType;
REFLECT_STRUCT_BEGIN(CubeRenderStepType, gfx::RenderStep)
REFLECT_STRUCT_MEMBER(m_material)
REFLECT_STRUCT_END(CubeRenderStepType)
