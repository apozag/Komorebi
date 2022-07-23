#include "BlendState.h"

BlendState::BlendState(Graphics& gfx, bool enable) {
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = enable;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	GetDevice(gfx)->CreateBlendState(&BlendState, &m_blendState);
}

void BlendState::Bind(Graphics& gfx)const {
	GetContext(gfx)->OMSetBlendState(m_blendState.Get(), NULL, 0xffffffff);
}
void BlendState::Unbind(Graphics& gfx)const {
	GetContext(gfx)->OMSetBlendState(NULL, NULL, 0xffffffff);
}