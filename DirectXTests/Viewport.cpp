#include "Viewport.h"

Viewport::Viewport(Graphics& gfx, int x, int y, int w, int h) {
	m_viewport = {};
	m_viewport.TopLeftX = x;
	m_viewport.TopLeftY = y;
	m_viewport.Width = w;
	m_viewport.Height = h;
	m_viewport.MaxDepth = 1;
	m_viewport.MinDepth = 0;
}

void Viewport::Bind(Graphics& gfx) const {
	GetContext(gfx)->RSSetViewports(1, &m_viewport);
}
void Viewport::Unbind(Graphics& gfx) const {
}