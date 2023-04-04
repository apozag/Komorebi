#pragma once

#include <string>
#include <vector>

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"
#include "Graphics\Renderer.h"

namespace gfx {

  class RenderStep : public GameObject {
  public:
    struct TextureInfo : public GameObject{
      std::string m_rtId;
      int m_textureIdx;
      REFLECT_BASE()
    };

    std::vector<TextureInfo> m_inputsInfo;
    std::string m_outRtId;
    unsigned int m_maxLayer = 0xFFFFFFFF;
    bool m_sortReverse = false;

    bool m_screenEffect = false;
    OWNED_PTR(Material) m_screenEffectMat;

    // Non serialized
    std::vector<const Texture2D*> m_inRts;
    const RenderTarget* m_outRt;

    RenderStep() {}
    RenderStep(std::vector<TextureInfo> inputs, const char* outid, unsigned int layer, bool screen, bool sort)
      : m_inputsInfo(inputs), m_outRtId(outid), m_maxLayer(layer), m_screenEffect(screen), m_sortReverse(sort) {
    };

    void Bind() const;
    void Execute(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int idx) const;
    void Unbind() const;

    REFLECT_BASE()

  private:
    void ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int idx) const;
  };

}

DECLARE_REFLECTION_VECTOR(gfx::RenderStep)
DECLARE_REFLECTION_VECTOR(gfx::RenderStep::TextureInfo)