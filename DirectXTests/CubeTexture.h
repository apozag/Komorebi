#pragma once

#include <string>
#include "Graphics.h"
#include "ResourceBindable.h"

class CubeTexture : public ResourceBindable{
public:
	CubeTexture(std::string path, unsigned int slot);
	~CubeTexture();
	void Bind()const override;
	void Unbind()const override;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	unsigned int m_slot;
};