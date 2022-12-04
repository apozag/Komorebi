#pragma once

#include <string>
#include "ResourceBindable.h"

class Texture3D : public ResourceBindable {
public:
	Texture3D(unsigned char* data, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format, unsigned int texelSize, unsigned int slot);
	~Texture3D();
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
};