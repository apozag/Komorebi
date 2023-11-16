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

    RenderInfo* renderInfo = Engine::GetRenderer()->GetRenderInfo();

    for (RenderStep::TextureInfo& texInfo : m_textureInputs) {
      const RenderTarget* rt = renderInfo->FindGlobalRenderTarget(texInfo.m_rtId);
      if (rt != nullptr) {
        // TODO: [ERROR] Texture index is higher than texture count
        Texture2D* tex = rt->GetTextures2D()[texInfo.m_textureIdx];
        m_inRts.push_back(tex);
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
    for (int i = 0; i < m_inRts.size(); i++) {
      const Texture2D* rt = m_inRts[i];
      const RenderStep::TextureInfo& texInfo = m_textureInputs[i];
      if (texInfo.m_slot < 0) {
        rt->Bind();
      }
      else {
        rt->BindAt(texInfo.m_slot);
      }
    }
    for (const ResourceBindable* resource : m_inResources) {
      resource->Bind();
    }
    if (m_outDS && m_outRt) {
      m_outRt->BindWithDS(m_outDS);
    }
    else if (m_outRt) {
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
}

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
REFLECT_STRUCT_MEMBER(m_slot)
REFLECT_STRUCT_END(TextureInfoType)

typedef gfx::RenderStep RenderStepType;
REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(RenderStepType)
REFLECT_STRUCT_MEMBER(m_textureInputs)
REFLECT_STRUCT_MEMBER(m_resourceInputs)
REFLECT_STRUCT_MEMBER(m_outRtId)
REFLECT_STRUCT_MEMBER(m_outDSId)
REFLECT_STRUCT_MEMBER(m_repeatFor)
REFLECT_STRUCT_END(RenderStepType)

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, RenderStep)

IMPLEMENT_REFLECTION_VECTOR(OWNED_PTR(gfx::RenderStep))
IMPLEMENT_REFLECTION_VECTOR(gfx::RenderStep::TextureInfo)
