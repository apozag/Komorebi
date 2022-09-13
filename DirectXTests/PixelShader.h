#pragma once

#include "StateBindable.h"

struct ID3D11ShaderReflection;

class PixelShader : public StateBindable{
public:
	PixelShader(const char* filename);
	~PixelShader();
	void Bind()const override;
	void Unbind()const override;
	void Update();
	ID3D11ShaderReflection * const GetReflection() const { return m_reflection.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection;
};
