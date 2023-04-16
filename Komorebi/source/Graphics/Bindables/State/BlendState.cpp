#include <d3d11.h>
#include "Graphics/Bindables/State/BlendState.h"

namespace gfx {

	BlendState::~BlendState() {
		//m_blendState->Release();
	}

	void BlendState::Setup() {
		D3D11_BLEND_DESC BlendState;
		ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
		BlendState.RenderTarget[0].BlendEnable = m_enable;
		BlendState.RenderTarget[0].SrcBlend = (D3D11_BLEND) m_srcBlendFactor;//D3D11_BLEND_SRC_ALPHA;
		BlendState.RenderTarget[0].DestBlend = (D3D11_BLEND)m_dstBlendFactor;//D3D11_BLEND_INV_SRC_ALPHA;
		BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		BlendState.RenderTarget[0].BlendOp = (D3D11_BLEND_OP) m_blendOp;//D3D11_BLEND_OP_ADD;
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
	REFLECT_STRUCT_MEMBER(m_srcBlendFactor)
	REFLECT_STRUCT_MEMBER(m_dstBlendFactor)
	REFLECT_STRUCT_MEMBER(m_blendOp)
	REFLECT_STRUCT_END(BlendState)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, BlendState)

typedef gfx::BlendState::BlendFactor BlendFactorEnum;
REFLECT_ENUM_BEGIN(BlendFactorEnum)
REFLECT_ENUM_VALUE(ZERO)
REFLECT_ENUM_VALUE(ONE)
REFLECT_ENUM_VALUE(SRC_COLOR)
REFLECT_ENUM_VALUE(INV_SRC_COLOR)
REFLECT_ENUM_VALUE(SRC_ALPHA)
REFLECT_ENUM_VALUE(INV_SRC_ALPHA)
REFLECT_ENUM_VALUE(DEST_ALPHA)
REFLECT_ENUM_VALUE(INV_DEST_ALPHA)
REFLECT_ENUM_VALUE(DEST_COLOR)
REFLECT_ENUM_VALUE(INV_DEST_COLOR)
REFLECT_ENUM_VALUE(SRC_ALPHA_SAT)
REFLECT_ENUM_VALUE(BLEND_FACTOR)
REFLECT_ENUM_VALUE(INV_BLEND_FACTOR)
REFLECT_ENUM_VALUE(SRC1_COLOR)
REFLECT_ENUM_VALUE(INV_SRC1_COLOR)
REFLECT_ENUM_VALUE(SRC1_ALPHA)
REFLECT_ENUM_VALUE(INV_SRC1_ALPHA)
REFLECT_ENUM_END(BlendFactorEnum)

typedef gfx::BlendState::BlendOp BlendOpEnum;
REFLECT_ENUM_BEGIN(BlendOpEnum)
REFLECT_ENUM_VALUE(ADD)
REFLECT_ENUM_VALUE(SUBTRACT)
REFLECT_ENUM_VALUE(REV_SUBTRACT)
REFLECT_ENUM_VALUE(MIN)
REFLECT_ENUM_VALUE(MAX)
REFLECT_ENUM_END(BlendOpEnum)
