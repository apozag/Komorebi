#pragma once

#include "Graphics/Bindables/State/StateBindable.h"

class BlendState : public StateBindable{
public:
	BlendState() {}
	BlendState(bool enable) : m_enable(enable) {}
	~BlendState();

	void Setup() override;

	void Bind()const override;
	void Unbind()const override;

	REFLECT()

private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

	//////////////////
	// Serializable
	//////////////////

	bool m_enable;

};