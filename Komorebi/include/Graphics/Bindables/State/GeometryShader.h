#pragma once

#include "Graphics\Bindables\State\StateBindable.h"

struct ID3D11GeometryShader;

namespace gfx{

  class GeometryShader : public StateBindable {
  public:

    GeometryShader() {}
    GeometryShader(std::string filename): m_filename(filename) {}

    void Setup() override;

    void Bind()const override;
    void Unbind()const override;
    void Update() {}

    REFLECT()

  private:

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

    std::string m_filename;
  };
}
