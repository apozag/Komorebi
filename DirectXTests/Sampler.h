#pragma once
#include "Bindable.h"
class Sampler : public Bindable{
public:
	Sampler(Graphics& gfx, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, int slot);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
	int m_slot;
};
