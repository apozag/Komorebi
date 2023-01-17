#include "Graphics/Bindables/State/SamplerState.h"
#include "Graphics/GraphicsThrowMacros.h"

SamplerState::~SamplerState() { 
	m_sampler->Release();
}

void SamplerState::Setup() {

	INFOMAN;

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = m_filter;// D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 0;
	desc.AddressU = m_mode;//D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = m_mode;//D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = m_mode;//D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.BorderColor[0] = 1.f;
	desc.BorderColor[1] = 1.f;
	desc.BorderColor[2] = 1.f;
	desc.BorderColor[3] = 1.f;
	//desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//desc.MaxLOD = 0;
	//desc.MinLOD = 0;

	GFX_THROW_INFO(GetDevice()->CreateSamplerState(&desc, &m_sampler));
}

void SamplerState::Bind( ) const {
	GetContext ()->PSSetSamplers(m_slot, 1, m_sampler.GetAddressOf());
}
void SamplerState::Unbind( ) const {
	ID3D11SamplerState* nullStates = { nullptr };
	GetContext ()->PSSetSamplers(m_slot, 1, &nullStates);
}

REFLECT_STRUCT_BEGIN(SamplerState, ResourceBindable)
REFLECT_STRUCT_MEMBER(m_filter)
REFLECT_STRUCT_MEMBER(m_mode)
REFLECT_STRUCT_END(SamplerState)

IMPLEMENT_REFLECTION_PRIMITIVE(D3D11_FILTER, D3D11_FILTER)
IMPLEMENT_REFLECTION_PRIMITIVE(D3D11_TEXTURE_ADDRESS_MODE, D3D11_TEXTURE_ADDRESS_MODE)