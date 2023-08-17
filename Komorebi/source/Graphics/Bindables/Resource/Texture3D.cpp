
#include "Graphics/GraphicsThrowMacros.h"
#include "Graphics/Bindables/Resource/Texture3D.h"
#include "Core/Util/ImageManager.h"

namespace wrl = Microsoft::WRL;

namespace gfx {

	Texture3D::Texture3D(unsigned char* data, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format, unsigned int texelSize, unsigned int slot) : ResourceBindable(slot) {
		INFOMAN

			D3D11_SUBRESOURCE_DATA sdata = {};
		sdata.pSysMem = data;
		sdata.SysMemPitch = texelSize * width;
		sdata.SysMemSlicePitch = texelSize * width * height;

		D3D11_TEXTURE3D_DESC desc = {};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = format;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MipLevels = 1;
		desc.Width = width;
		desc.Height = height;
		desc.Depth = depth;
		wrl::ComPtr<ID3D11Texture3D> pTexture;
		GFX_THROW_INFO(GetDevice()->CreateTexture3D(&desc, &sdata, &pTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.Texture3D.MipLevels = 1;
		srvDesc.Texture3D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		GFX_THROW_INFO(GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, m_srv.GetAddressOf()));		
	}

	Texture3D::~Texture3D() {
		//m_srv->Release();
	}

	void Texture3D::Bind() const {
		GetContext()->PSSetShaderResources(m_slot, 1u, m_srv.GetAddressOf());
	}

	void Texture3D::Unbind() const {
		static ID3D11ShaderResourceView* nullViews[] = { nullptr };
		GetContext()->PSSetShaderResources(m_slot, 1u, nullViews);
	}
}
