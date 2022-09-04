#pragma once

#include "StateBindable.h"
#include "VertexShader.h"

class InputLayout : public StateBindable {
public:
	InputLayout(const D3D11_INPUT_ELEMENT_DESC* ied, unsigned int numElements, const VertexShader& vs);
	~InputLayout();
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
