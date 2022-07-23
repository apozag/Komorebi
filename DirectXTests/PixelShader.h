#pragma once

#include "Bindable.h"

class PixelShader : public Bindable{
public:
	PixelShader(Graphics& gfx, const char* filename);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
	void Update(Graphics& gfx);
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};
