#include "Graphics\RenderPipeline\RenderStep.h"
#include "Core/Engine.h"
#include "Core/Math\Transform.h"
#include "Graphics\Renderer.h"
#include "Graphics\Bindables\Resource\Texture2D.h"
#include "Graphics\Bindables\Resource\RenderTarget.h"
#include "Graphics\Material.h"
#include "Entities\Drawable.h"

namespace gfx {

  void RenderStep::Bind() const {
    for (const Texture2D* rt : m_inRts) {
      rt->Bind();
    }
    if (m_outRt) { 
      m_outRt->Bind(); 
    }
  }

  void RenderStep::Execute(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int idx) const {

    Bind();

    switch (m_type) {
    case DEFAULT:
    {
      Pass* lastPass = nullptr;
      Material* lastMat = nullptr;
      for (int i = idx; i < jobsToExecute; i++) {

        Job job = jobs[i];

        if (job.pass->m_layer > m_maxLayer) {
          break;
        }

        Pass* pass = job.pass;
        if (pass != lastPass) {
          if (lastPass) lastPass->Unbind();
          pass->Bind();
          lastPass = pass;
          //stateBindCount++;
        }

        Material* material = job.material;
        if (material != lastMat) {
          if (lastMat) lastMat->Unbind();
          material->Bind();
          lastMat = material;
          //resourceBindCount++;
        }

        job.drawable->Draw(DirectX::XMMatrixTranspose(job.transform->GetMatrix()));

        lastPass = job.pass;
        lastMat = job.material;
      }
    }
    break;
    case CLEAR:
    {
      m_outRt->Clear(0, 0, 0);
    }
    break;
    case SCREEN:
    {
      if (m_screenEffectMat) {
        m_screenEffectMat->Bind();
        for (Pass* pass : m_screenEffectMat->GetPasses()) {
          pass->Bind();
          Engine::GetRenderer()->GetQuadPrimitive()->Draw(DirectX::XMMatrixIdentity());
          pass->Unbind();
        }
        m_screenEffectMat->Bind();
      }
    }
    break;
    }

    Unbind();
  }

  void RenderStep::Unbind() const {
    for (const Texture2D* rt : m_inRts) {
      rt->Unbind();
    }
    if (m_outRt) m_outRt->Unbind();
  }
}

typedef gfx::RenderStep::Type RenderStepTypeEnum;
REFLECT_ENUM_BEGIN(RenderStepTypeEnum)
REFLECT_ENUM_VALUE(DEFAULT)
REFLECT_ENUM_VALUE(CLEAR)
REFLECT_ENUM_VALUE(SCREEN)
REFLECT_ENUM_END(RenderStepTypeEnum)

typedef gfx::RenderStep::TextureInfo TextureInfoType;
REFLECT_STRUCT_BASE_BEGIN(TextureInfoType)
REFLECT_STRUCT_MEMBER(m_rtId)
REFLECT_STRUCT_MEMBER(m_textureIdx)
REFLECT_STRUCT_END(TextureInfoType)

typedef gfx::RenderStep RenderStepType;
REFLECT_STRUCT_BASE_BEGIN(RenderStepType)
REFLECT_STRUCT_MEMBER(m_type)
REFLECT_STRUCT_MEMBER(m_inputsInfo)
REFLECT_STRUCT_MEMBER(m_outRtId)
REFLECT_STRUCT_MEMBER(m_maxLayer)
REFLECT_STRUCT_MEMBER(m_sortReverse)
REFLECT_STRUCT_MEMBER(m_screenEffectMat)
REFLECT_STRUCT_END(RenderStepType)

IMPLEMENT_REFLECTION_VECTOR(gfx::RenderStep)
IMPLEMENT_REFLECTION_VECTOR(gfx::RenderStep::TextureInfo)
