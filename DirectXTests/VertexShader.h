#pragma once

#include "StateBindable.h"

struct ID3D11ShaderReflection;

class VertexShader : public StateBindable {
public:
	VertexShader(const char* filename);
	~VertexShader();
	void Bind()const override;
	void Unbind()const override;
	void Update();
	ID3DBlob* GetBytecode() const;
	ID3D11ShaderReflection* const GetReflection() const { return m_reflection.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection;
};