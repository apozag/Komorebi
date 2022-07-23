#include <d3dcompiler.h>
#include"WideStringUtility.h"
#include "PixelShader.h"
#include "GraphicsThrowMacros.h"

PixelShader::PixelShader(Graphics& gfx, const char* filename) {
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(filename).c_str(), &pBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf()));
}
void PixelShader::Bind(Graphics& gfx) const {
	GetContext(gfx)->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
}
void PixelShader::Unbind(Graphics& gfx) const {
}
void PixelShader::Update(Graphics& gfx)
{

}