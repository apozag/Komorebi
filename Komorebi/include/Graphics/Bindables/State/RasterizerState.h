#pragma once

#include "Graphics/Bindables/State/StateBindable.h"

struct ID3D11RasterizerState;

namespace gfx {

	class RasterizerState : public StateBindable {
	public: 

		enum CullMode {						
			NONE,
			FRONT,
			BACK
		};

	public:
		RasterizerState(CullMode cullMode = CullMode::BACK, bool wireframe = false) : m_cullMode(cullMode), m_wireframe(wireframe) {}
		~RasterizerState();

		void Setup() override;

		void Bind()const override;
		void Unbind()const override;

		REFLECT()

	private:

		static RasterizerState* GetDefault();

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer;

		//////////////////
		// Serializable
		//////////////////

		CullMode m_cullMode;
		bool m_wireframe;
	};
}

DECLARE_REFLECTION_ENUM(gfx::RasterizerState::CullMode);
