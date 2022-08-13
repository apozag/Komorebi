#pragma once
#include "Bindable.h"
class Sampler : public Bindable{
public:
	Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, int slot);
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
	int m_slot;
};
