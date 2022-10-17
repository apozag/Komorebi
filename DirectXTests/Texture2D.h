#pragma once

#include <string>

#include "ResourceBindable.h"

class Texture2D : public ResourceBindable {
public:
	Texture2D(std::string path, unsigned int slot);
	Texture2D(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels, unsigned int slot);
	Texture2D(Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, unsigned int slot);
	~Texture2D();
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
};