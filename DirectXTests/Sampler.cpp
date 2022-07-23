#include "Sampler.h"
#include "GraphicsThrowMacros.h"

Sampler::Sampler(Graphics& gfx) {
	INFOMAN(gfx);

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 0;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	//desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//desc.MaxLOD = 0;
	//desc.MinLOD = 0;

	GFX_THROW_INFO( GetDevice(gfx)->CreateSamplerState(&desc, &m_sampler));
}

void Sampler::Bind(Graphics& gfx) const {
	GetContext(gfx)->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
}
void Sampler::Unbind(Graphics& gfx) const {
}