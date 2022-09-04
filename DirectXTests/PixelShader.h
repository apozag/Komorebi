#pragma once

#include "StateBindable.h"

class PixelShader : public StateBindable{
public:
	PixelShader(const char* filename);
	~PixelShader();
	void Bind()const override;
	void Unbind()const override;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};
