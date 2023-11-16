#pragma once

#include "Graphics/RenderPipeline/RenderStep.h"

namespace gfx {

  class ComputeShader;

  template<typename T>
  class ConstantBuffer;

  class RenderTarget;

  class DownsampleRenderStep : public RenderStep { 
  public:

    unsigned int m_maxLayer = 0;
    bool m_sortReverse = false;
    ASSET_PTR(Material) m_screenEffectMat;

    DownsampleRenderStep() {}
    DownsampleRenderStep(std::vector<TextureInfo> inputs, const char* outid)
      : RenderStep(inputs, outid) {
    };

    void Setup() override;

    REFLECT()

  private:
    void ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const override;

    int m_iterations = 0;

    Material* m_downMat;
    Material* m_upMat;
    
    std::vector<RenderTarget*> m_auxRts;
  };

}
