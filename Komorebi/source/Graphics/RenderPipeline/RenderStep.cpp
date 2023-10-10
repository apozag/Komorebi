#include "Graphics\RenderPipeline\RenderStep.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Engine.h"
#include "Core/Math\Transform.h"
#include "Graphics\Renderer.h"
#include "Graphics\RenderInfo.h"
#include "Graphics\Bindables\Resource\Texture2D.h"
#include "Graphics\Bindables\Resource\RenderTarget.h"
#include "Graphics\Material.h"
#include "Graphics\MaterialInstance.h"
#include "Entities\Drawable.h"
#include "Entities\Light.h"

namespace gfx {

  void RenderStep::Setup() {

    const RenderInfo* renderInfo = Engine::GetRenderer()->GetRenderInfo();

    for (RenderStep::TextureInfo& texInfo : m_textureInputs) {
      const RenderTarget* rt = renderInfo->FindGlobalRenderTarget(texInfo.m_rtId);
      if (rt != nullptr) {
        // TODO: [ERROR] Texture index is higher than texture count
        m_inRts.push_back(rt->GetTextures2D()[texInfo.m_textureIdx]);
      }
    }

    for (const std::string& resName : m_resourceInputs) {
      const ResourceBindable* resource = renderInfo->FindGlobalResource(resName);
      if (resource != nullptr) {        
        m_inResources.push_back(resource);
      }
    }

    if (m_outRtId == "DEFAULT") {
      m_outRt = Engine::GetDefaultRendertarget();
    }
    else {
      m_outRt = renderInfo->FindGlobalRenderTarget(m_outRtId);
    }

    if (!m_outDSId.empty()) {
      if (m_outDSId == "DEFAULT") {
        m_outDS = Engine::GetDefaultRendertarget();
      }
      else {
        m_outDS = renderInfo->FindGlobalRenderTarget(m_outDSId);
      }
    }
    else {
      m_outDS = nullptr;
    }
  }

  void RenderStep::Bind() const {
    for (const Texture2D* rt : m_inRts) {
      rt->Bind();
    }
    for (const ResourceBindable* resource : m_inResources) {
      resource->Bind();
    }
    if (m_outDS && m_outRt){
      m_outRt->BindWithDS(m_outDS);
    }
    else if(m_outRt){
      m_outRt->Bind();
    }
  }

  void RenderStep::Unbind() const {
    for (const Texture2D* rt : m_inRts) {
      rt->Unbind();
    }
    for (const ResourceBindable* resource : m_inResources) {
      resource->Unbind();
    }
    if (m_outRt) m_outRt->Unbind();
  }

  void RenderStep::Execute(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const {

    Bind();

    if (m_repeatFor == RepeatFor::ONCE) {
      ExecuteInternal(jobs, jobsToExecute, startIdx, endIdx);
    } 
    else {
      if ((m_repeatFor & RepeatFor::DIRLIGHT) != 0) {
        for (const DirectionalLight* light : Engine::GetRenderer()->GetDirLights()) {
          light->Bind();
          ExecuteInternal(jobs, jobsToExecute, startIdx, endIdx);
          light->Unbind();
        }
      }
      if ((m_repeatFor & RepeatFor::SPOTLIGHT) != 0) {
        for (const SpotLight* light : Engine::GetRenderer()->GetSpotLights()) {
          light->Bind();
          ExecuteInternal(jobs, jobsToExecute, startIdx, endIdx);
          light->Unbind();
        }
      }
      if ((m_repeatFor & RepeatFor::POINTLIGHT) != 0) {
        for (const PointLight* light : Engine::GetRenderer()->GetPointLights()) {
          light->Bind();
          ExecuteInternal(jobs, jobsToExecute, startIdx, endIdx);
          light->Unbind();
        }
      }
    }    

    Unbind();
  }

  void RenderStep::ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const {
    switch (m_type) {
    case DEFAULT:
    {
      const Pass* lastPass = nullptr;
      const Material* lastMat = nullptr;
      for (int i = startIdx; i < jobsToExecute; i++) {

        Job job = jobs[i];

        if (job.pass->m_layer > m_maxLayer) {
          endIdx = i;
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
      if(lastPass) lastPass->Unbind();
      if(lastMat) lastMat->Unbind();
    }
    break;
    case CLEAR:
    {
      if (m_outRt) 
      {
        m_outRt->Clear(0, 0, 0, 0);
      }
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
        m_screenEffectMat->Unbind();
      }
    }
    break;
    case CUBE:
    {
      if (m_screenEffectMat) {
        m_screenEffectMat->Bind();
        for (Pass* pass : m_screenEffectMat->GetPasses()) {
          pass->Bind();
          Engine::GetRenderer()->GetCubePrimitive()->Draw(DirectX::XMMatrixIdentity());
          pass->Unbind();
        }
        m_screenEffectMat->Unbind();
      }
    }
    break;
    }
  }  
}

typedef gfx::RenderStep::Type RenderStepTypeEnum;
REFLECT_ENUM_BEGIN(RenderStepTypeEnum)
REFLECT_ENUM_VALUE(DEFAULT)
REFLECT_ENUM_VALUE(CLEAR)
REFLECT_ENUM_VALUE(SCREEN)
REFLECT_ENUM_VALUE(CUBE)
REFLECT_ENUM_END(RenderStepTypeEnum)

typedef gfx::RenderStep::RepeatFor RepeatForEnum;
REFLECT_ENUM_BEGIN(RepeatForEnum)
REFLECT_ENUM_VALUE(ONCE)
REFLECT_ENUM_VALUE(DIRLIGHT)
REFLECT_ENUM_VALUE(SPOTLIGHT)
REFLECT_ENUM_VALUE(POINTLIGHT)
REFLECT_ENUM_END(RepeatForEnum)

IMPLEMENT_REFLECTION_BITMASK(RepeatForEnum)

typedef gfx::RenderStep::TextureInfo TextureInfoType;
REFLECT_STRUCT_BASE_BEGIN(TextureInfoType)
REFLECT_STRUCT_MEMBER(m_rtId)
REFLECT_STRUCT_MEMBER(m_textureIdx)
REFLECT_STRUCT_END(TextureInfoType)

typedef gfx::RenderStep RenderStepType;
REFLECT_STRUCT_BASE_BEGIN(RenderStepType)
REFLECT_STRUCT_MEMBER(m_type)
REFLECT_STRUCT_MEMBER(m_textureInputs)
REFLECT_STRUCT_MEMBER(m_resourceInputs)
REFLECT_STRUCT_MEMBER(m_outRtId)
REFLECT_STRUCT_MEMBER(m_outDSId)
REFLECT_STRUCT_MEMBER(m_maxLayer)
REFLECT_STRUCT_MEMBER(m_sortReverse)
REFLECT_STRUCT_MEMBER(m_repeatFor)
REFLECT_STRUCT_MEMBER(m_screenEffectMat)
REFLECT_STRUCT_END(RenderStepType)

IMPLEMENT_REFLECTION_VECTOR(gfx::RenderStep)
IMPLEMENT_REFLECTION_VECTOR(gfx::RenderStep::TextureInfo)
