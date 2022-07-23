#pragma once

#include "Bindable.h"

class VertexShader : public Bindable {
public:
	VertexShader(Graphics& gfx, const char* filename);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
	void Update(Graphics& gfx);
	ID3DBlob* GetBytecode() const;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_bytecodeBlob;
};