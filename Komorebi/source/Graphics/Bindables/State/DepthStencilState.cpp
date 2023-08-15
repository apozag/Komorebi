#include "Graphics/Bindables/State/DepthStencilState.h"

#include "Graphics/GraphicsThrowMacros.h"
#include "Core/Reflection/ReflectionImplMacros.h"

typedef gfx::DepthStencilState::DepthStencilAccess DepthStencilAccessEnum;
REFLECT_ENUM_BEGIN(DepthStencilAccessEnum)
REFLECT_ENUM_VALUE(DEPTH_WRITE)
REFLECT_ENUM_VALUE(DEPTH_READ)
REFLECT_ENUM_VALUE(STENCIL_WRITE)
REFLECT_ENUM_VALUE(STENCIL_READ)
REFLECT_ENUM_END(DepthStencilAccessEnum)

IMPLEMENT_REFLECTION_BITMASK(DepthStencilAccessEnum)

namespace gfx {

	DepthStencilState::~DepthStencilState() {
		//m_pDSState->Release();
	}

	void DepthStencilState::Setup() {
		INFOMAN

			// Depth-Stencil State
			D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.DepthEnable = m_DSAccess & (DepthStencilAccess::DEPTH_READ | DepthStencilAccess::DEPTH_WRITE);

		if (m_DSAccess & DepthStencilAccess::DEPTH_WRITE) {
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		}
		else {
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		}

		if (m_DSAccess & DepthStencilAccess::DEPTH_READ) {
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		}
		else {
			dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		}

		dsDesc.StencilEnable = m_DSAccess & (DepthStencilAccess::STENCIL_READ | DepthStencilAccess::STENCIL_WRITE);
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.StencilReadMask = 0xFF;

		if (m_DSAccess & DepthStencilAccess::STENCIL_WRITE) {
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		}
		else {
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		}

		if (m_DSAccess & DepthStencilAccess::STENCIL_READ) {
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

		GFX_THROW_INFO(GetDevice()->CreateDepthStencilState(&dsDesc, m_pDSState.GetAddressOf()));
	}

	void DepthStencilState::Reconfigure() {
		m_pDSState->Release();
		Setup();
	}

	void DepthStencilState::Bind() const {
		GetContext()->OMSetDepthStencilState(m_pDSState.Get(), m_refVal);
	}

	REFLECT_STRUCT_BEGIN(DepthStencilState, StateBindable)
	REFLECT_STRUCT_MEMBER(m_DSAccess)
	REFLECT_STRUCT_MEMBER(m_refVal)
	REFLECT_STRUCT_END(DepthStencilState)
}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, DepthStencilState)
