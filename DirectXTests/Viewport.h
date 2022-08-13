#pragma once

#include "Bindable.h"

class Viewport : public Bindable{
public:
	Viewport(int x, int y, int w, int h);
	void Bind()const override;
	void Unbind()const override;
private:
	D3D11_VIEWPORT m_viewport;
};
