#include "Graphics/Bindables/Resource/CubeTexture.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Util/ImageManager.h"

namespace gfx {
	CubeTexture::CubeTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, unsigned int slot) : Texture2D("", slot) {
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, m_srv.GetAddressOf());
	}


	void CubeTexture::Setup() {
		std::vector<Image> images(6);
		D3D11_SUBRESOURCE_DATA sd[6];
		for (int i = 0; i < 6; i++) {
			images[i] = ImageManager::loadImage(m_filename + "/" + std::to_string(i) + ".png");
			sd[i].pSysMem = images[i].data;
			sd[i].SysMemPitch = images[i].getMemPitch();
			//sd[i].SysMemSlicePitch = 0;
		}
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = images[0].width;
		texDesc.Height = images[0].height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 6;
		texDesc.Format = images[0].format;
		texDesc.CPUAccessFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;

		GetDevice()->CreateTexture2D(&texDesc, &sd[0], m_texture.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srv.GetAddressOf());
	}

	REFLECT_STRUCT_BEGIN(CubeTexture, Texture2D)
		REFLECT_STRUCT_END(CubeTexture)
}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, CubeTexture)
