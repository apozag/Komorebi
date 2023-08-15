#pragma once

#include <string>
#include "Graphics/Graphics.h"
#include "Graphics/Bindables/Resource/Texture2D.h"

namespace gfx {

	class CubeTexture : public Texture2D {
	public:
		CubeTexture() {}
		CubeTexture(std::string path, unsigned int slot) : Texture2D(path, slot) {}		
		CubeTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, unsigned int slot);

		virtual void Setup() override;

		REFLECT()
	};

}

DECLARE_REFLECTION_POINTER(gfx::CubeTexture)