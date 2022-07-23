#include "DepthStencilState.h"
#include "GraphicsThrowMacros.h"

DepthStencilState::DepthStencilState(Graphics& gfx, int dsst, int refVal) : m_refVal(refVal) {
	INFOMAN(gfx);

	// Depth-Stencil State
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = dsst & (DepthStencilAccess::DEPTH_READ | DepthStencilAccess::DEPTH_WRITE);

	if (dsst & DepthStencilAccess::DEPTH_WRITE) {
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	}
	else {
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	if (dsst & DepthStencilAccess::DEPTH_READ) {
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	}
	else {
		dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	}

	dsDesc.StencilEnable = dsst & (DepthStencilAccess::STENCIL_READ | DepthStencilAccess::STENCIL_WRITE);
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.StencilReadMask = 0xFF;

	if (dsst & DepthStencilAccess::STENCIL_WRITE) {
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	}
	else {
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	}

	if (dsst & DepthStencilAccess::STENCIL_READ) {
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	}
	else {
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	}

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilState(&dsDesc, m_pDSState.GetAddressOf()));
}

void DepthStencilState::Bind(Graphics& gfx) const {
	GetContext(gfx)->OMSetDepthStencilState(m_pDSState.Get(), m_refVal);
}