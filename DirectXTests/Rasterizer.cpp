#include "Rasterizer.h"

#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))

Rasterizer::Rasterizer( bool cullFront, bool wireframe) {
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = wireframe? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	desc.CullMode = cullFront? D3D11_CULL_FRONT : D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;
	desc.DepthBias = 0;// DEPTH_BIAS_D32_FLOAT(-0.00001);
	desc.DepthBiasClamp = 0;//-0.001;
	desc.SlopeScaledDepthBias = 0;// 1.f;
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;
	desc.AntialiasedLineEnable = true;

	GetDevice ()->CreateRasterizerState(&desc, &m_rasterizer);
}

void Rasterizer::Bind( ) const {
	GetContext ()->RSSetState(m_rasterizer.Get());
}
void Rasterizer::Unbind( ) const {
}