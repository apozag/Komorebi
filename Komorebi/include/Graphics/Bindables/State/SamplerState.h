#pragma once
#include "Graphics/Bindables/Resource/ResourceBindable.h"
class SamplerState : public ResourceBindable{
public:
	SamplerState(){}
	SamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, int slot) : ResourceBindable(slot), m_filter(filter), m_mode(mode) {}
	~SamplerState();

	void Setup() override;

	void Bind()const override;
	void Unbind()const override;

	REFLECT()

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;

	//////////////////
	// Serializable
	//////////////////

	D3D11_FILTER m_filter;
	D3D11_TEXTURE_ADDRESS_MODE m_mode;
};

DECLARE_REFLECTION_PRIMITIVE(D3D11_FILTER)
DECLARE_REFLECTION_PRIMITIVE(D3D11_TEXTURE_ADDRESS_MODE)
