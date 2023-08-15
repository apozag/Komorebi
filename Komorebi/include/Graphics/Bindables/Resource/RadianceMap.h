#pragma once

#include "Graphics\Bindables\Resource\ResourceBindable.h"

namespace gfx {

  class CubeRenderTarget;

  class RadianceMap : public ResourceBindable {
  public:
    RadianceMap() {}
    RadianceMap(std::string filename, unsigned int slot) : ResourceBindable(slot), m_filename(filename) {}

    ~RadianceMap();

    virtual void Setup() override;  

    virtual void Bind() const override;
    virtual void Unbind() const override;

    REFLECT()

  private:

    CubeRenderTarget* m_diffuseRadMap;
    CubeRenderTarget* m_specularRadMap;

    std::string m_filename;

    unsigned int m_size = 512;
    
  };

}

DECLARE_REFLECTION_POINTER(gfx::RadianceMap)
