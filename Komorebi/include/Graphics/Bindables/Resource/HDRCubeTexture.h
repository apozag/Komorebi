#pragma once

#include "Graphics\Bindables\Resource\CubeTexture.h"

namespace gfx {

  class CubeRenderTarget;

  class HDRCubeTexture : public CubeTexture {
  public:
    HDRCubeTexture() {}
    HDRCubeTexture(std::string path, unsigned int slot) : CubeTexture(path, slot) {}

    ~HDRCubeTexture();

    virtual void Setup() override;  

    REFLECT()

  private:

    CubeRenderTarget* m_cubeRT;

    unsigned int m_size = 512;
    
  };

}

DECLARE_REFLECTION_POINTER(gfx::HDRCubeTexture)
