#pragma once

#include "Graphics/RenderPipeline/RenderStep.h"

namespace gfx {

  class ClearRenderStep : public RenderStep {
  public:

    unsigned int m_maxLayer = 0;
    bool m_sortReverse = false;
    ASSET_PTR(Material) m_screenEffectMat;

    ClearRenderStep() {}
    ClearRenderStep(std::vector<TextureInfo> inputs, const char* outid)
      : RenderStep(inputs, outid) {
    };

    void Bind() const override {}
    void Unbind() const override {}

    REFLECT()

  private:
    void ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const override;
  };

}
