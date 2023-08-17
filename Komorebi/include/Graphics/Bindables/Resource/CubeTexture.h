#pragma once

#include <string>
#include "Graphics/Graphics.h"
#include "Graphics/Bindables/Resource/Texture2D.h"

namespace gfx {

	class CubeTexture : public Texture2D {
	public:
		CubeTexture() {}
		CubeTexture(std::string path, unsigned int slot, unsigned int mipLevels = 1) : Texture2D(path, slot, mipLevels) {}
		CubeTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, unsigned int slot, unsigned int mipLevels = 1);

		virtual void Setup() override;

		void GenMipMaps();

		REFLECT()
	};

}

DECLARE_REFLECTION_POINTER(gfx::CubeTexture)