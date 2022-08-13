#pragma once

#include "Bindable.h"

class BlendState : public Bindable{
public:
	BlendState(bool enable);
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
};