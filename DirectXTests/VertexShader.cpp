#include <d3dcompiler.h>
#include "VertexShader.h"
#include "WideStringUtility.h"
#include "GraphicsThrowMacros.h"

VertexShader::VertexShader( const char* filename) {
	INFOMAN;
	GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(filename).c_str(), &m_bytecodeBlob));
	GFX_THROW_INFO(GetDevice ()->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(), m_bytecodeBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf()));
}
void VertexShader::Bind( ) const {
	GetContext ()->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
}
void VertexShader::Unbind( ) const {
}
void VertexShader::Update( ) {
}

ID3DBlob* VertexShader::GetBytecode() const {
	return m_bytecodeBlob.Get();
}
