#pragma once

#include <string>

#include "Bindable.h"

class Texture2D : public Bindable {
public:
	Texture2D(Graphics& gfx, std::string path, unsigned int idx);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	unsigned int m_idx;
};