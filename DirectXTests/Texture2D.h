#pragma once

#include <string>

#include "ResourceBindable.h"

struct Image;

class Texture2D : public ResourceBindable {
public:
	Texture2D(){}
	Texture2D(std::string filename, unsigned int slot) : ResourceBindable(slot), m_filename(filename) {}
	Texture2D(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels, unsigned int slot);
	Texture2D(Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, unsigned int slot);
	~Texture2D();

	void Setup() override;

	void Bind()const override;
	void Unbind()const override;

	REFLECT()

private:

	void CreateFromImage(const Image& image);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;

	//////////////////
	// Serializable
	//////////////////

	std::string m_filename;
};