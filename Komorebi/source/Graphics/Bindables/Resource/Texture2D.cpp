#include "Graphics/GraphicsThrowMacros.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Core/Util/ImageManager.h"

namespace wrl = Microsoft::WRL;

namespace gfx {

	Texture2D::Texture2D(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels, unsigned int slot, unsigned int mipLevels) : ResourceBindable(slot), m_mipLevels(mipLevels) {

		Image image;
		image.data = data;
		image.width = width;
		image.height = height;
		image.channels = channels;
		image.format = DXGI_FORMAT_R8G8B8A8_UNORM;

		CreateFromImage(image);
	}

	Texture2D::Texture2D(wrl::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, unsigned int slot, unsigned int mipLevels) : ResourceBindable(slot), m_mipLevels(mipLevels) {
		INFOMAN

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_mipLevels;		

		GFX_THROW_INFO(GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, m_srv.GetAddressOf()));

		/*if (m_mipLevels != 1) {
			GetContext()->GenerateMips(m_srv.Get());
		}*/
	}

	Texture2D::~Texture2D() {
		//m_srv->Release();
	}

	void Texture2D::Setup() {
		Image img = ImageManager::loadImage(m_filename);
		CreateFromImage(img);
	}

	void Texture2D::Bind() const {
		GetContext()->PSSetShaderResources(m_slot, 1u, m_srv.GetAddressOf());
	}

	void Texture2D::BindAt(unsigned int slot) const {
		GetContext()->PSSetShaderResources(slot, 1u, m_srv.GetAddressOf());
	}

	void Texture2D::Unbind() const {
		static ID3D11ShaderResourceView* nullViews[] = { nullptr };
		GetContext()->PSSetShaderResources(m_slot, 1u, nullViews);
	}

	void Texture2D::UnbindAt(unsigned int slot) const {
		static ID3D11ShaderResourceView* nullViews[] = { nullptr };
		GetContext()->PSSetShaderResources(slot, 1u, nullViews);
	}

	void Texture2D::CreateFromImage(const Image& image) {
		INFOMAN

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = image.data;
		data.SysMemPitch = image.getMemPitch();

		D3D11_TEXTURE2D_DESC desc = {};
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = image.format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MipLevels = m_mipLevels;
		desc.Width = image.width;
		desc.Height = image.height;

		if (m_mipLevels != 1) {
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		wrl::ComPtr<ID3D11Texture2D> pTexture;
		if (m_mipLevels == 1) {
			GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, &data, &pTexture));
		}
		else {
			GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, NULL, &pTexture));
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = image.format;
		srvDesc.Texture2D.MipLevels = m_mipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		GFX_THROW_INFO(GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, m_srv.GetAddressOf()));

		if (m_mipLevels != 1) {
			GetContext()->UpdateSubresource(pTexture.Get(), 0u, NULL, image.data, image.getMemPitch(), 0u);
			GetContext()->GenerateMips(m_srv.Get());
		}
	}

	void Texture2D::GenMipMaps() {
		GetContext()->GenerateMips(m_srv.Get());
	}

	REFLECT_STRUCT_BEGIN(Texture2D, ResourceBindable)
	REFLECT_STRUCT_MEMBER(m_filename)
	REFLECT_STRUCT_MEMBER(m_mipLevels)
	REFLECT_STRUCT_END(Texture2D)
}