#pragma once

#include <string>
#include "Graphics.h"
#include "Bindable.h"

class CubeTexture : public Bindable{
public:
	CubeTexture(Graphics& gfx, std::string path, unsigned int slot);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
	void Update(Graphics& gfx);
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	unsigned int m_slot;
};