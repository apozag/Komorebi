#pragma once

#include <D3d11shader.h>

#include "Graphics/Bindables/State/StateBindable.h"

//struct ID3D11ShaderReflection;

class PixelShader : public StateBindable {
public:
	PixelShader() {}
	PixelShader(const char* filename) : m_filename(filename) {}
	~PixelShader();

	void Setup() override;

	void Bind()const override;
	void Unbind()const override {}
	void Update() {}
	ID3D11ShaderReflection * const GetShaderReflection() const { return m_reflection.Get(); }

	REFLECT()

private:

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection;

	////////////////////////
	// Serializable
	////////////////////////

	std::string m_filename;
};

DECLARE_REFLECTION_POINTER(PixelShader)
