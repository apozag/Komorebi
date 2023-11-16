#pragma once

#include <string>
#include <vector>

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"
#include "Graphics/Renderer.h"

namespace gfx {

  class Material;
  class ResourceBindable;
  struct Job;

  class RenderStep : public GameObject {
  public:

    enum RepeatFor {
      ONCE = 0,
      DIRLIGHT = 1,
      SPOTLIGHT = 2,
      POINTLIGHT = 4,
    };

    struct TextureInfo : public GameObject{
      std::string m_rtId;
      int m_textureIdx;
      int m_slot = -1;
      REFLECT_BASE()
    };

    std::vector<TextureInfo> m_textureInputs;
    std::vector<std::string> m_resourceInputs;
    std::string m_outRtId;
    std::string m_outDSId = "";    
    BITMASK(RepeatFor) m_repeatFor = 0u;    

    // Non serialized
    std::vector<const Texture2D*> m_inRts;
    std::vector<const ResourceBindable*> m_inResources;
    RenderTarget* m_outRt;
    RenderTarget* m_outDS;

    RenderStep() {}
    RenderStep(std::vector<TextureInfo> inputs, const char* outid)
      : m_textureInputs(inputs), m_outRtId(outid){
    };

    virtual void Setup() override;

    virtual void Bind() const;
    void Execute(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const;
    virtual void Unbind() const;

    REFLECT_BASE()

  private:
    virtual void ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const = 0;
  };

}

DECLARE_REFLECTION_ENUM(gfx::RenderStep::RepeatFor)
DECLARE_REFLECTION_BITMASK(gfx::RenderStep::RepeatFor)

DECLARE_REFLECTION_POINTER(gfx::RenderStep)

DECLARE_REFLECTION_VECTOR(OWNED_PTR(gfx::RenderStep))
DECLARE_REFLECTION_VECTOR(gfx::RenderStep::TextureInfo)