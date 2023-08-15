#pragma once

#include "Graphics\Bindables\Resource\RenderTarget.h"

namespace gfx {

  class CubeTexture;

  class CubeRenderTarget : public RenderTarget {

  public:
    CubeRenderTarget() {}
    CubeRenderTarget(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int slot) :
      RenderTarget(width, height, format, 1, slot)
    {}    

    virtual void Setup() override;

    CubeTexture* GetCubeTexture();      
    
  };

}
