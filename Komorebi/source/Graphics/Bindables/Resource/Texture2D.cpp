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

	Texture2D::Texture2D(wrl::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, int width, int height, unsigned int slot, unsigned int mipLevels) : ResourceBindable(slot), m_mipLevels(mipLevels) {
		INFOMAN

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_mipLevels;		

		m_texture = pTexture;		

		GFX_THROW_INFO(GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srv.GetAddressOf()));

		m_format = format;
		m_width = width;
		m_height = height;		
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

	void Texture2D::BindAt(unsigned int slot, int mipLevel, unsigned int stageMask) const {
		INFOMAN

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
		if (mipLevel >= 0) {
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = m_format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = (unsigned int) mipLevel;
			srvDesc.Texture2D.MipLevels = 1u;
			GFX_THROW_INFO(GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, srv.GetAddressOf()));
		}
		else {
			srv = m_srv;
		}

		if ((stageMask & PipelineStage::VERTEX) != 0) {
			GetContext()->VSSetShaderResources(slot, 1u, srv.GetAddressOf());
		}
		if ((stageMask & PipelineStage::GEOMETRY) != 0) {
			GetContext()->GSSetShaderResources(slot, 1u, srv.GetAddressOf());
		}
		if ((stageMask & PipelineStage::PIXEL) != 0) {
			GetContext()->PSSetShaderResources(slot, 1u, srv.GetAddressOf());
		}
		if ((stageMask & PipelineStage::COMPUTE) != 0) {
			GetContext()->CSSetShaderResources(slot, 1u, srv.GetAddressOf());
		}		
	}

	void Texture2D::Unbind() const {
		static ID3D11ShaderResourceView* nullViews[] = { nullptr };
		GetContext()->PSSetShaderResources(m_slot, 1u, nullViews);		
	}

	void Texture2D::UnbindAt(unsigned int slot, unsigned int stageMask) const {
		static ID3D11ShaderResourceView* nullViews[] = { nullptr };	

		if ((stageMask & PipelineStage::VERTEX) != 0) {
			GetContext()->VSSetShaderResources(slot, 1u, nullViews);
		}
		if ((stageMask & PipelineStage::GEOMETRY) != 0) {
			GetContext()->GSSetShaderResources(slot, 1u, nullViews);
		}
		if ((stageMask & PipelineStage::PIXEL) != 0) {
			GetContext()->PSSetShaderResources(slot, 1u, nullViews);
		}
		if ((stageMask & PipelineStage::COMPUTE) != 0) {
			GetContext()->CSSetShaderResources(slot, 1u, nullViews);
		}
	}

	void Texture2D::BindAsUAV(unsigned int slot, unsigned int mipSlice) const {
		static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;

		D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Texture2D.MipSlice = mipSlice;
		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		desc.Format = m_format;
		desc.Buffer.FirstElement = 0u;
		desc.Buffer.NumElements = m_width * m_height;
		desc.Buffer.Flags = 0u;
		GetDevice()->CreateUnorderedAccessView(m_texture.Get(), &desc, uav.ReleaseAndGetAddressOf());

		GetContext()->CSSetUnorderedAccessViews(slot, 1u, uav.GetAddressOf(), 0u);
	}

	void Texture2D::UnbindUAV(unsigned int slot) const {
		static ID3D11UnorderedAccessView* nullViews[] = { nullptr };
		GetContext()->CSSetUnorderedAccessViews(slot, 1u, nullViews, 0u);
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
		
		if (m_mipLevels == 1) {
			GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, &data, m_texture.GetAddressOf()));
		}
		else {
			GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, NULL, m_texture.GetAddressOf()));
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = image.format;
		srvDesc.Texture2D.MipLevels = m_mipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		GFX_THROW_INFO(GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srv.GetAddressOf()));

		if (m_mipLevels != 1) {
			GetContext()->UpdateSubresource(m_texture.Get(), 0u, NULL, image.data, image.getMemPitch(), 0u);
			GetContext()->GenerateMips(m_srv.Get());
		}

		m_format = image.format;
		m_width = image.width;
		m_height = image.height;
	}

	void Texture2D::GenMipMaps() {
		GetContext()->GenerateMips(m_srv.Get());
	}

	REFLECT_STRUCT_BEGIN(Texture2D, ResourceBindable)
	REFLECT_STRUCT_MEMBER(m_filename)
	REFLECT_STRUCT_MEMBER(m_mipLevels)
	REFLECT_STRUCT_END(Texture2D)
}