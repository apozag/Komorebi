#pragma once

#include <d3d11.h>

#include "Graphics/Bindables/State/StateBindable.h"

//struct D3D11_VIEWPORT;

namespace gfx {

	class Viewport : public StateBindable {
	public:
		Viewport() {}
		Viewport(int x, int y, int w, int h) : m_x(x), m_y(y), m_h(h), m_w(w) {};
		void Setup() override;
		void Bind()const override;
		void Unbind()const override;
		REFLECT()
	private:
		int m_x, m_y, m_w, m_h;
		D3D11_VIEWPORT m_viewport;
	};
}

DECLARE_REFLECTION_POINTER(gfx::Viewport)
