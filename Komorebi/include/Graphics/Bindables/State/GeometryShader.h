#pragma once

#include <D3d11shader.h>

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

    ID3D11ShaderReflection* const GetShaderReflection() const { return m_reflection.Get(); }

    REFLECT()

  private:

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection;

    std::string m_filename;
  };
}
