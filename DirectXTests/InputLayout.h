#pragma once

#include "Bindable.h"
#include "VertexShader.h"

class InputLayout : public Bindable {
public:
	InputLayout(const D3D11_INPUT_ELEMENT_DESC* ied, unsigned int numElements, const VertexShader& vs);
	~InputLayout();
	void Bind()const override;
	void Unbind()const override;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
