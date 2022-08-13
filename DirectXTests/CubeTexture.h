#pragma once

#include <string>
#include "Graphics.h"
#include "Bindable.h"

class CubeTexture : public Bindable{
public:
	CubeTexture(std::string path, unsigned int slot);
	void Bind()const override;
	void Unbind()const override;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	unsigned int m_slot;
};