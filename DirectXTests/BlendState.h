#pragma once

#include "Bindable.h"

class BlendState : public Bindable{
public:
	BlendState(Graphics& gfx, bool enable);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
};