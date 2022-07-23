#pragma once

#include "Bindable.h"

class Rasterizer : public Bindable{
public:
	Rasterizer(Graphics& gfx, bool cullFront = false, bool wireframe = false);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer;
};