#pragma once

#include <string>

#include "Bindable.h"

class Texture2D : public Bindable {
public:
	Texture2D(std::string path, unsigned int slot);
	~Texture2D();
	void Bind()const override;
	void Unbind()const override;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	unsigned int m_slot;
};