#include "d3dcompiler.h"
#include <D3d11shader.h>
#include "WideStringUtility.h"
#include "PixelShader.h"
#include "GraphicsThrowMacros.h"

PixelShader::PixelShader( const char* filename) {
	INFOMAN;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(filename).c_str(), &pBlob));

	GFX_THROW_INFO(D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)m_reflection.GetAddressOf()));

	GFX_THROW_INFO(GetDevice ()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf()));
}
PixelShader::~PixelShader() {
	m_pixelShader->Release();
	m_reflection->Release();
}

void PixelShader::Bind( ) const {
	GetContext ()->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
}
void PixelShader::Unbind( ) const {
}
void PixelShader::Update( )
{

}