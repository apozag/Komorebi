#include "RenderTarget.h"
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

RenderTarget::RenderTarget(Graphics& gfx, IDXGISwapChain* swapChain) : m_idx(0){
	INFOMAN(gfx);

	// Get back buffer
	wrl::ComPtr<ID3D11RenderTargetView> rtv;
	ID3D11Texture2D* pBackBuffer = nullptr;
	GFX_THROW_INFO(swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
	GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(pBackBuffer, nullptr, rtv.GetAddressOf()));
	m_rtv.push_back(rtv);

	D3D11_TEXTURE2D_DESC rtDesc;
	pBackBuffer->GetDesc(&rtDesc);

	// Depth-Stencil Texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = rtDesc.Width;
	descDepth.Height = rtDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, NULL, pDepthStencil.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, m_dsv.GetAddressOf()));

	m_width = rtDesc.Width;
	m_height = rtDesc.Height;

}

RenderTarget::RenderTarget(Graphics& gfx, int width, int height, DXGI_FORMAT format, int count) : m_idx(0), m_width(width), m_height(height){
	INFOMAN(gfx);	

	// Depth-Stencil Texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, NULL, pDepthStencil.GetAddressOf()));

	// Depth Stencil View 
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, m_dsv.GetAddressOf()));

	// Depth Shader Resource View
	wrl::ComPtr<ID3D11ShaderResourceView> dssrv;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, dssrv.GetAddressOf()));

	// Texture description
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Format = format;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	for (int i = 0; i < count; i++) {

		// Create Texture 2D
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, NULL, pTexture.GetAddressOf()));

		// Create Render Target View
		wrl::ComPtr<ID3D11RenderTargetView> rtv;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(pTexture.Get(), &rtvDesc, rtv.GetAddressOf()));

		m_rtv.push_back(rtv);

		// Shader Resource View
		wrl::ComPtr<ID3D11ShaderResourceView> srv;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, srv.GetAddressOf());		

		m_srv.push_back(srv);
	}

	m_srv.push_back(dssrv);
}

void RenderTarget::Bind(Graphics& gfx) const {

	if (locked) return;

	if (isShaderResource) {
		GetContext(gfx)->PSSetShaderResources(m_idx, m_srv.size(), m_srv[0].GetAddressOf());
	}
	else {
		//ID3D11RenderTargetView* const * srv = (m_rtv.size() ? m_rtv[0].GetAddressOf() : nullptr);
		if (m_rtv.size()) {
			GetContext(gfx)->OMSetRenderTargets(m_rtv.size(), m_rtv[0].GetAddressOf(), m_dsv.Get());
		}
		else {
			GetContext(gfx)->OMSetRenderTargets(m_rtv.size(), nullptr, m_dsv.Get());
		}
	}
}
void RenderTarget::Unbind(Graphics& gfx) const {

	if (locked) return;

	if (isShaderResource) {
		std::vector<ID3D11ShaderResourceView*> nullViews(m_srv.size(), nullptr);
		GetContext(gfx)->PSSetShaderResources(m_idx, m_srv.size(), nullViews.data());
	}
	/*
	else {
		if (m_rtv.size()) {
			std::vector<ID3D11ShaderResourceView*> nullViews(m_rtv.size(), nullptr);
			GetContext(gfx)->OMSetRenderTargets(m_rtv.size(), nullViews.data());
		}
		else {
			GetContext(gfx)->OMSetRenderTargets(1, nullptr, nullptr);
		}
	}
	*/
}

void RenderTarget::Clear(Graphics& gfx, float r, float g, float b) {

	if (locked) return;

	INFOMAN_NOHR(gfx);
	const float color[] = { r, g, b, 1.0f };
	for (wrl::ComPtr<ID3D11RenderTargetView> rtv : m_rtv) {
		GFX_THROW_INFO_ONLY(GetContext(gfx)->ClearRenderTargetView(rtv.Get(), color));
	}
	GFX_THROW_INFO_ONLY(GetContext(gfx)->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0u));
}

void RenderTarget::SetAsShaderResource() {
	isShaderResource = true;
}

void RenderTarget::SetAsRenderTarget() {
	isShaderResource = false;
}

void RenderTarget::Lock() {
	locked = true;
}
void RenderTarget::Unlock() {
	locked = false;
}
