#include "Graphics/Bindables/State/Viewport.h"

Viewport::Viewport( int x, int y, int w, int h) {
	m_viewport = {};
	m_viewport.TopLeftX = x;
	m_viewport.TopLeftY = y;
	m_viewport.Width = w;
	m_viewport.Height = h;
	m_viewport.MaxDepth = 1;
	m_viewport.MinDepth = 0;
}

void Viewport::Bind( ) const {
	GetContext ()->RSSetViewports(1, &m_viewport);
}
void Viewport::Unbind( ) const {
}