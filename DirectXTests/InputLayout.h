#pragma once

#include "Bindable.h"
#include "VertexShader.h"

class InputLayout : public Bindable {
public:
	InputLayout(Graphics& gfx, const D3D11_INPUT_ELEMENT_DESC* ied, unsigned int numElements, const VertexShader& vs);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
	void Update(Graphics& gfx);
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
