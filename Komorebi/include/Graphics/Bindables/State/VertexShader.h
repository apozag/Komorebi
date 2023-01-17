#pragma once

#include <D3d11shader.h>

#include "Graphics/Bindables/State/StateBindable.h"

//struct ID3D11ShaderReflection;

class VertexShader : public StateBindable {
public:
	VertexShader() {}
	VertexShader(const char* filename) : m_filename(filename) {}
	~VertexShader();

	void Setup() override;

	void Bind()const override;
	void Unbind()const override;
	void Update();
	ID3DBlob* GetBytecode() const;
	ID3D11ShaderReflection* const GetShaderReflection() const { return m_reflection.Get(); }

	REFLECT()

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection;

	//////////////////
	// Serializable
	//////////////////

	std::string m_filename;
};

DECLARE_REFLECTION_POINTER(VertexShader)