#pragma once

#include "StateBindable.h"

class BlendState : public StateBindable{
public:
	BlendState(bool enable);
	~BlendState();
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
};