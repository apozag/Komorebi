#pragma once

#include <vector>
#include <string>

#include "Graphics\RenderPipeline.h"
#include "Graphics\Bindables\Resource\RenderTarget.h"
#include "Core/Reflection/ReflectionMacros.h"

namespace gfx {

  class ResourceBindable;

  class RenderInfo : public GameObject {  
    friend class Renderer;
  public:
    struct RTNamePair : public GameObject {
      std::string m_name;
      RenderTarget m_rt;
      RTNamePair() {}
      RTNamePair(const char* name, RenderTarget&& rt) : m_name(name), m_rt(rt) {}
      REFLECT_BASE()
    };
    struct ResourceNamePair : public GameObject {
      std::string m_name;
      OWNED_PTR(ResourceBindable) m_resource;
      REFLECT_BASE()
    };

    RenderPipeline* FindRenderPipeline(const std::string& name);
    RenderTarget* FindGlobalRenderTarget(const std::string& name);
    const ResourceBindable* FindGlobalResource(const std::string& name) const;
    const std::vector<RTNamePair>& GetGlobalRenderTargets() const { return m_globalRTs; }
    const std::vector<ResourceNamePair>& GetGlobalResources() const { return m_globalResources; }

    REFLECT_BASE()

  private:
    std::vector<RenderPipeline> m_renderPipelines;
    std::vector<RTNamePair> m_globalRTs;
    std::vector<ResourceNamePair> m_globalResources;
  };

}

DECLARE_REFLECTION_VECTOR(gfx::RenderPipeline)
DECLARE_REFLECTION_VECTOR(gfx::RenderInfo::RTNamePair)
DECLARE_REFLECTION_VECTOR(gfx::RenderInfo::ResourceNamePair)

