#pragma once

#include "Bindable.h"

class VertexShader : public Bindable {
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