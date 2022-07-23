#pragma once

#include "Bindable.h"

class Viewport : public Bindable{
public:
	Viewport(Graphics& gfx, int x, int y, int w, int h);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
private:
	D3D11_VIEWPORT m_viewport;
};
