#include "Graphics/Bindables/State/RasterizerState.h"

#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))

RasterizerState::~RasterizerState() {
	m_rasterizer->Release();
}

void RasterizerState::Setup() {

	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = m_wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	desc.CullMode = m_cullFront ? D3D11_CULL_FRONT : D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;
	desc.DepthBias = 0;// DEPTH_BIAS_D32_FLOAT(-0.00001);
	desc.DepthBiasClamp = 0;//-0.001;
	desc.SlopeScaledDepthBias = 0;// 1.f;
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;
	desc.AntialiasedLineEnable = true;

	GetDevice()->CreateRasterizerState(&desc, &m_rasterizer);
}

void RasterizerState::Bind( ) const {
	GetContext ()->RSSetState(m_rasterizer.Get());
}
void RasterizerState::Unbind( ) const {
}

REFLECT_STRUCT_BEGIN(RasterizerState, StateBindable)
REFLECT_STRUCT_MEMBER(m_cullFront)	
REFLECT_STRUCT_MEMBER(m_wireframe)	
REFLECT_STRUCT_END(RasterizerState)