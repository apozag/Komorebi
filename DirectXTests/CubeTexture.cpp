#include "CubeTexture.h"
#include "ImageManager.h"

CubeTexture::CubeTexture(std::string path, unsigned int slot) : m_slot(slot) {
	std::vector<Image> images(6);
	D3D11_SUBRESOURCE_DATA sd[6];
	for (int i = 0; i < 6; i++) {
		images[i] = ImageManager::loadImage(path + "/" + std::to_string(i) + ".png");
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

void CubeTexture::Bind() const {
	GetContext()->PSSetShaderResources(m_slot, 1, m_srv.GetAddressOf());
}
void CubeTexture::Unbind() const {
	ID3D11ShaderResourceView* nullViews[] = { nullptr };
	GetContext()->PSSetShaderResources(m_slot, 1, nullViews);
}

void CubeTexture::Update() {

}