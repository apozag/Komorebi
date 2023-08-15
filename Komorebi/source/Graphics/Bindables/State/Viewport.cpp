#include "Graphics/Bindables/State/Viewport.h"

#include "Core/Reflection/ReflectionImplMacros.h"

namespace gfx {	

	void Viewport::Setup() {
		m_viewport = {};
		m_viewport.TopLeftX = m_x;
		m_viewport.TopLeftY = m_y;
		m_viewport.Width = m_w;
		m_viewport.Height = m_h;
		m_viewport.MaxDepth = 1;
		m_viewport.MinDepth = 0;
	}

	void Viewport::Bind() const {
		GetContext()->RSSetViewports(1, &m_viewport);
	}
	void Viewport::Unbind() const {
	}

	REFLECT_STRUCT_BEGIN(Viewport, StateBindable)
	REFLECT_STRUCT_MEMBER(m_x)
	REFLECT_STRUCT_MEMBER(m_y)
	REFLECT_STRUCT_MEMBER(m_w)
	REFLECT_STRUCT_MEMBER(m_h)
	REFLECT_STRUCT_END(Viewport, StateBindable)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, Viewport)
