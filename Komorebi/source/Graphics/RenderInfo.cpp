#include "Graphics/RenderInfo.h"

namespace gfx {

  const RenderPipeline* RenderInfo::FindRenderPipeline(const std::string& name) const {
    for (const RenderPipeline& pipeline : m_renderPipelines) {
      if (pipeline.GetName() == name) {
        return &pipeline;
      }
    }
    // TODO: [ERROR] Pipeline "name" does not exist
    return nullptr;
  }

  const RenderTarget* RenderInfo::FindGlobalRenderTarget(const std::string& name) const {
    for (const RTNamePair& pair : m_globalRTs) {
      if (pair.m_name == name) {
        return &pair.m_rt;
      }
    }
    // TODO: [ERROR] RenderTarget "name" does not exist
    return nullptr;
  }
  
  const ResourceBindable* RenderInfo::FindGlobalResource(const std::string& name) const {
    for (const ResourceNamePair& pair : m_globalResources) {
      if (pair.m_name == name) {
        return pair.m_resource;
      }
    }
    // TODO: [ERROR] ResourceBindable "name" does not exist
    return nullptr;
  }

  typedef RenderInfo::RTNamePair RTNamePairType;
  REFLECT_STRUCT_BASE_BEGIN(RTNamePairType)
  REFLECT_STRUCT_MEMBER(m_name)
  REFLECT_STRUCT_MEMBER(m_rt)
  REFLECT_STRUCT_END(RTNamePairType)

  typedef RenderInfo::ResourceNamePair ResourceNamePairType;
  REFLECT_STRUCT_BASE_BEGIN(ResourceNamePairType)
  REFLECT_STRUCT_MEMBER(m_name)
  REFLECT_STRUCT_MEMBER(m_resource)
  REFLECT_STRUCT_END(ResourceNamePairType)

  REFLECT_STRUCT_BASE_BEGIN(RenderInfo)
  REFLECT_STRUCT_MEMBER(m_renderPipelines)
  REFLECT_STRUCT_MEMBER(m_globalRTs)
  REFLECT_STRUCT_MEMBER(m_globalResources)
  REFLECT_STRUCT_END(RenderInfo)

}

IMPLEMENT_REFLECTION_VECTOR(gfx::RenderPipeline)
IMPLEMENT_REFLECTION_VECTOR(gfx::RenderInfo::RTNamePair)
IMPLEMENT_REFLECTION_VECTOR(gfx::RenderInfo::ResourceNamePair)