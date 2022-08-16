#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

InputLayout::InputLayout(const D3D11_INPUT_ELEMENT_DESC* ied, unsigned int numElements, const VertexShader& vs) {
	INFOMAN;
	ID3DBlob* pBlob = vs.GetBytecode();
	GFX_THROW_INFO(GetDevice()->CreateInputLayout(ied, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), m_inputLayout.GetAddressOf()));
}

InputLayout::~InputLayout() {
	m_inputLayout->Release();
}

void InputLayout::Bind() const {
	GetContext()->IASetInputLayout(m_inputLayout.Get());
}

void InputLayout::Unbind() const {
}

void InputLayout::Update()
{

}