#pragma once

#include "StateBindable.h"

class VertexShader : public StateBindable {
public:
	VertexShader(const char* filename);
	~VertexShader();
	void Bind()const override;
	void Unbind()const override;
	void Update();
	ID3DBlob* GetBytecode() const;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
};