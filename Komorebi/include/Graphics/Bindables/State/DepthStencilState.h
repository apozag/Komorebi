#pragma once

#include "Graphics/Bindables/State/StateBindable.h"

struct ID3D11DepthStencilState;

namespace gfx {

	class DepthStencilState : public StateBindable {
	public:
		enum DepthStencilAccess {
			NONE					= 0x0,
			DEPTH_WRITE		= 0x1,
			DEPTH_READ		= 0x2,
			STENCIL_WRITE = 0X4,
			STENCIL_READ	= 0x8
		};
	public:

		DepthStencilState() {}
		DepthStencilState(int dsst, int refVal = 0) : m_refVal(refVal), m_DSAccess(dsst) {}
		~DepthStencilState();

		void Setup() override;
		void Reconfigure() override;

		void Bind()const override;
		void Unbind()const override {}

		REFLECT()

	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDSState;

		//////////////////
		// Serializable
		//////////////////

		BITMASK(DepthStencilAccess) m_DSAccess;
		unsigned int m_refVal;
	};

}

DECLARE_REFLECTION_ENUM(gfx::DepthStencilState::DepthStencilAccess)
DECLARE_REFLECTION_BITMASK(gfx::DepthStencilState::DepthStencilAccess)

DECLARE_REFLECTION_POINTER(gfx::DepthStencilState)