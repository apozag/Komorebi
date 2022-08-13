#pragma once

#include "Bindable.h"

class PixelShader : public Bindable{
public:
	PixelShader(const char* filename);
	void Bind()const override;
	void Unbind()const override;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};
