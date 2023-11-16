#include "Graphics/RenderPipeline/DownsampleRenderStep.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Engine.h"
#include "Core/Memory/Factory.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderInfo.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/Resource/ConstantBuffer.h"
#include "Graphics/Bindables/State/ComputeShader.h"
#include "Graphics/Material.h"
#include "Graphics/PipelineStage.h"
#include "Core/PrefabManager.h"
#include "Entities\Drawable.h"

namespace gfx {

  void DownsampleRenderStep::Setup() {
    RenderStep::Setup();    

    m_downMat = PrefabManager::GetInstance()->LoadPrefab<Material>("assets/materials/downsampleMat.xml");
    m_upMat = PrefabManager::GetInstance()->LoadPrefab<Material>("assets/materials/upsampleMat.xml");

    if (m_outRt != nullptr) {
      unsigned int w = m_outRt->GetWidth();
      unsigned int h = m_outRt->GetHeight();

      // Ensure the number of iterations does not exceed the divisions we can make of the original texture
      int maxIters = ((int)log2f(max(w, h)));
      m_iterations = min(m_iterations, maxIters);

      for (int i = 1; i <= m_iterations; i++) {
        RenderTarget* rt = memory::Factory::Create<RenderTarget>(w >> i, h >> i, m_outRt->GetFormat(), 1u, 0u, 1u, false, false);
        rt->Setup();
        m_auxRts.push_back(rt);
      }
    }
  }

  void DownsampleRenderStep::ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const {
    if (m_outRt == nullptr) return;
    
    float texelSize[2] = { 0.f, 0.f };    

    Texture2D* srcTex = nullptr;
    RenderTarget* dstRt = m_outRt;

    for(int i = 0; i < m_iterations; i++) {
      srcTex = dstRt->GetTextures2D()[0];
      dstRt = m_auxRts[i];
      texelSize[0] = 1.f / (float) srcTex->GetWidth();
      texelSize[1] = 1.f / (float) srcTex->GetHeight();
      m_downMat->SetVector2("texelSize", &texelSize[0]);
      Renderer::Blit(srcTex, dstRt, m_downMat);      
    }       

    for (int i = m_iterations-2; i >= 0; i--) {
      srcTex = dstRt->GetTextures2D()[0];
      dstRt = m_auxRts[i];
      texelSize[0] = 1.f / (float) srcTex->GetWidth();
      texelSize[1] = 1.f / (float) srcTex->GetHeight();
      m_upMat->SetVector2("texelSize", &texelSize[0]);
      Renderer::Blit(srcTex, dstRt, m_upMat);
    }

    Renderer::Blit(dstRt->GetTextures2D()[0], m_outRt, m_upMat);
  }
}

typedef gfx::DownsampleRenderStep DownsampleRenderStepType;
REFLECT_STRUCT_BEGIN(DownsampleRenderStepType, gfx::RenderStep)
REFLECT_STRUCT_MEMBER(m_iterations)
REFLECT_STRUCT_END(DownsampleRenderStepType)
