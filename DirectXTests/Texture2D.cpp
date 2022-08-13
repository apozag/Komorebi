#include "GraphicsThrowMacros.h"
#include "Texture2D.h"
#include "ImageManager.h"

namespace wrl = Microsoft::WRL;

Texture2D::Texture2D( std::string path, unsigned int slot): m_slot(slot) {
	INFOMAN;

	Image img = ImageManager::loadImage(path);

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = img.data;
	data.SysMemPitch = img.getMemPitch();

	D3D11_TEXTURE2D_DESC desc = {};
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = img.format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.MipLevels = 1;
	desc.Width = img.width;
	desc.Height = img.height;	
	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice ()->CreateTexture2D(&desc, &data, &pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = img.format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	GFX_THROW_INFO(GetDevice ()->CreateShaderResourceView(pTexture.Get(), &srvDesc, m_srv.GetAddressOf()));
}

void Texture2D::Bind( ) const {
	GetContext ()->PSSetShaderResources(m_slot, 1u, m_srv.GetAddressOf());
}

void Texture2D::Unbind( ) const {
	ID3D11ShaderResourceView* nullViews[] = { nullptr };
	GetContext ()->PSSetShaderResources(m_slot, 1u, nullViews);
}
