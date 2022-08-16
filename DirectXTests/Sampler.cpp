#include "Sampler.h"
#include "GraphicsThrowMacros.h"

Sampler::Sampler( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, int slot) : m_slot(slot){
	INFOMAN;

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = filter;// D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 0;
	desc.AddressU = mode;//D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = mode;//D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = mode;//D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.BorderColor[0] = 1.f;
	desc.BorderColor[1] = 1.f;
	desc.BorderColor[2] = 1.f;
	desc.BorderColor[3] = 1.f;
	//desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//desc.MaxLOD = 0;
	//desc.MinLOD = 0;

	GFX_THROW_INFO( GetDevice ()->CreateSamplerState(&desc, &m_sampler));
}
Sampler::~Sampler() {
	m_sampler->Release();
}

void Sampler::Bind( ) const {
	GetContext ()->PSSetSamplers(m_slot, 1, m_sampler.GetAddressOf());
}
void Sampler::Unbind( ) const {
	ID3D11SamplerState* nullStates = { nullptr };
	GetContext ()->PSSetSamplers(m_slot, 1, &nullStates);
}