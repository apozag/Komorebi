#pragma once

#include "Bindable.h"

class DepthStencilState : public Bindable {
public:
	enum DepthStencilAccess {
		DEPTH_WRITE = 0x1,
		DEPTH_READ = 0x2,
		STENCIL_WRITE = 0X4,
		STENCIL_READ = 0x8
	};
public:
	DepthStencilState(Graphics& gfx, int dsst, int refVal=0);
	void Update(Graphics& gfx){}
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override {}
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDSState;
	unsigned int m_refVal;
};