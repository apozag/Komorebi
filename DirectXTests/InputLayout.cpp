#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

InputLayout::InputLayout(Graphics& gfx, const D3D11_INPUT_ELEMENT_DESC* ied, unsigned int numElements, const VertexShader& vs) {
	INFOMAN(gfx);
	ID3DBlob* pBlob = vs.GetBytecode();
	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(ied, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), m_inputLayout.GetAddressOf()));
}

void InputLayout::Bind(Graphics& gfx) const {
	GetContext(gfx)->IASetInputLayout(m_inputLayout.Get());
}

void InputLayout::Unbind(Graphics& gfx) const {
}

void InputLayout::Update(Graphics& gfx)
{

}