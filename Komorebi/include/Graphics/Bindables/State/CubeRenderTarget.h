#pragma once

#include "Graphics\Bindables\Resource\RenderTarget.h"

namespace gfx {

  class CubeTexture;

  class CubeRenderTarget : public RenderTarget {

  public:
    CubeRenderTarget() {}
    CubeRenderTarget(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int slot, unsigned int mipLevels = 1u) :
      RenderTarget(width, height, format, 1, slot, mipLevels)
    {}    

    virtual void Setup() override;

    CubeTexture* GetCubeTexture();      
    
  };

}
