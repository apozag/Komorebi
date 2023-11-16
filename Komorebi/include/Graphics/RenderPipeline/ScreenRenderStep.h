#pragma once

#include "Graphics/RenderPipeline/RenderStep.h"

namespace gfx {

  class ScreenRenderStep : public RenderStep {
  public:    

    ScreenRenderStep() {}
    ScreenRenderStep(std::vector<TextureInfo> inputs, const char* outid)
      : RenderStep(inputs, outid) {
    };

    REFLECT()

  private:    

    void ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const override;

    ASSET_PTR(Material) m_material;
  };

}
