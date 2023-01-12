#include "BlendState.h"

BlendState::~BlendState() {
	m_blendState->Release();
}

void BlendState::Setup() {
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = m_enable;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	GetDevice()->CreateBlendState(&BlendState, &m_blendState);
}

void BlendState::Bind()const {
	GetContext()->OMSetBlendState(m_blendState.Get(), NULL, 0xffffffff);
}
void BlendState::Unbind()const {
	GetContext()->OMSetBlendState(NULL, NULL, 0xffffffff);
}

REFLECT_STRUCT_BEGIN(BlendState, StateBindable)
REFLECT_STRUCT_MEMBER(m_enable)
REFLECT_STRUCT_END(BlendState)