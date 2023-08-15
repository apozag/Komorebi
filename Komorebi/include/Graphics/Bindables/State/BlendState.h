#pragma once

#include "Graphics/Bindables/State/StateBindable.h"

struct ID3D11BlendState;

namespace gfx {

	class BlendState : public StateBindable {
	public:
		enum BlendFactor {
			ZERO						= D3D11_BLEND_ZERO,
			ONE							= D3D11_BLEND_ONE,
			SRC_COLOR				= D3D11_BLEND_SRC_COLOR,
			INV_SRC_COLOR		= D3D11_BLEND_INV_SRC_COLOR,
			SRC_ALPHA				= D3D11_BLEND_SRC_ALPHA,
			INV_SRC_ALPHA		= D3D11_BLEND_INV_SRC_ALPHA,
			DEST_ALPHA			= D3D11_BLEND_DEST_ALPHA,
			INV_DEST_ALPHA	= D3D11_BLEND_INV_DEST_ALPHA,
			DEST_COLOR			= D3D11_BLEND_DEST_COLOR,
			INV_DEST_COLOR	= D3D11_BLEND_INV_DEST_COLOR ,
			SRC_ALPHA_SAT		= D3D11_BLEND_SRC_ALPHA_SAT ,
			BLEND_FACTOR		= D3D11_BLEND_BLEND_FACTOR ,
			INV_BLEND_FACTOR = D3D11_BLEND_INV_BLEND_FACTOR ,
			SRC1_COLOR			= D3D11_BLEND_SRC1_COLOR ,
			INV_SRC1_COLOR	= D3D11_BLEND_INV_SRC1_COLOR ,
			SRC1_ALPHA			= D3D11_BLEND_SRC1_ALPHA ,
			INV_SRC1_ALPHA	= D3D11_BLEND_INV_SRC1_ALPHA
		};

		enum BlendOp {

			ADD						= D3D11_BLEND_OP_ADD,
			SUBTRACT			= D3D11_BLEND_OP_SUBTRACT,
			REV_SUBTRACT	= D3D11_BLEND_OP_REV_SUBTRACT,
			MIN						= D3D11_BLEND_OP_MIN,
			MAX						= D3D11_BLEND_OP_MAX
		};

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

		BlendFactor m_srcBlendFactor;
		BlendFactor m_dstBlendFactor;
		BlendOp m_blendOp;
	};
}

DECLARE_REFLECTION_POINTER(gfx::BlendState)

DECLARE_REFLECTION_ENUM(gfx::BlendState::BlendFactor)
DECLARE_REFLECTION_ENUM(gfx::BlendState::BlendOp)
