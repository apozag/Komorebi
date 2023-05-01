#pragma once

#include <string>
#include <vector>

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"
#include "Graphics\Renderer.h"

namespace gfx {

  class Material;

  class RenderStep : public GameObject {
  public:

    enum Type {
      DEFAULT,
      CLEAR,
      SCREEN
    };

    enum RepeatFor {
      ONCE = 0,
      DIRLIGHT = 1,
      SPOTLIGHT = 2,
      POINTLIGHT = 4,
    };

    struct TextureInfo : public GameObject{
      std::string m_rtId;
      int m_textureIdx;
      REFLECT_BASE()
    };

    Type m_type;

    std::vector<TextureInfo> m_inputsInfo;
    std::string m_outRtId;
    unsigned int m_maxLayer = 0;
    bool m_sortReverse = false;
    BITMASK(RepeatFor) m_repeatFor = 0u;

    OWNED_PTR(Material) m_screenEffectMat;

    // Non serialized
    std::vector<const Texture2D*> m_inRts;
    const RenderTarget* m_outRt;

    RenderStep() {}
    RenderStep(Type type, std::vector<TextureInfo> inputs, const char* outid, unsigned int layer, bool sort)
      : m_type(type), m_inputsInfo(inputs), m_outRtId(outid), m_maxLayer(layer), m_sortReverse(sort) {
    };

    void Bind() const;
    void Execute(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int idx) const;
    void Unbind() const;

    REFLECT_BASE()

  private:
    void ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int idx) const;
  };

}

DECLARE_REFLECTION_ENUM(gfx::RenderStep::Type)
DECLARE_REFLECTION_ENUM(gfx::RenderStep::RepeatFor)

DECLARE_REFLECTION_BITMASK(gfx::RenderStep::RepeatFor)

DECLARE_REFLECTION_VECTOR(gfx::RenderStep)
DECLARE_REFLECTION_VECTOR(gfx::RenderStep::TextureInfo)