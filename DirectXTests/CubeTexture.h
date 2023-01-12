#pragma once

#include <string>
#include "Graphics.h"
#include "ResourceBindable.h"

class CubeTexture : public ResourceBindable{
public:
	CubeTexture() {}
	CubeTexture(std::string path, unsigned int slot);
	~CubeTexture();

	void Setup() override;

	void Bind()const override;
	void Unbind()const override;
	void Update();

	REFLECT()

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;

	//////////////////
	// Serializable
	//////////////////

	std::string m_path;
	unsigned int m_slot;
};