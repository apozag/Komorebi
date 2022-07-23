#pragma once
#include "Bindable.h"
class Sampler : public Bindable{
public:
	Sampler(Graphics& gfx);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
};
