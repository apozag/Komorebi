#pragma once

#include "Graphics/RenderPipeline/RenderStep.h"

namespace gfx {  

  class GeometryRenderStep : public RenderStep {
  public:        

    GeometryRenderStep() {}
    GeometryRenderStep(std::vector<TextureInfo> inputs, const char* outid, unsigned int maxLayer, bool sortReverse)
      : RenderStep(inputs,outid), m_maxLayer(maxLayer), m_sortReverse(sortReverse) {
    };

    unsigned int GetMaxLayer() const { return m_maxLayer; }
    bool GetSortReverse() const { return m_sortReverse; }

    REFLECT()      

  private:
    void ExecuteInternal(std::vector<Job>& jobs, unsigned int jobsToExecute, unsigned int startIdx, unsigned int& endIdx) const override;

    unsigned int m_maxLayer = 0;
    bool m_sortReverse = false;
  };

}
