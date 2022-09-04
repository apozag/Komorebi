#pragma once
#include "ResourceBindable.h"
class Sampler : public ResourceBindable{
public:
	Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, int slot);
	~Sampler();
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
};
