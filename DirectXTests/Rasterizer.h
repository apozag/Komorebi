#pragma once

#include "StateBindable.h"

class Rasterizer : public StateBindable{
public:
	Rasterizer(bool cullFront = false, bool wireframe = false);
	~Rasterizer();
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer;
};