#include <d3dcompiler.h>
#include "VertexShader.h"
#include "WideStringUtility.h"
#include "GraphicsThrowMacros.h"

VertexShader::VertexShader(Graphics& gfx, const char* filename) {
	INFOMAN(gfx);
	GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(filename).c_str(), &m_bytecodeBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(), m_bytecodeBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf()));
}
void VertexShader::Bind(Graphics& gfx) const {
	GetContext(gfx)->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
}
void VertexShader::Unbind(Graphics& gfx) const {
}
void VertexShader::Update(Graphics& gfx) {
}

ID3DBlob* VertexShader::GetBytecode() const {
	return m_bytecodeBlob.Get();
}
