#include <assert.h>
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Core\Memory\Factory.h"
#include "Graphics/GraphicsThrowMacros.h"
#include "Graphics/Bindables/Resource/Texture2D.h"

namespace wrl = Microsoft::WRL;

namespace gfx {

	std::unordered_map<int, int> boundShaderResources;
	int idCount;

	RenderTarget::RenderTarget(IDXGISwapChain* swapChain) {

		INFOMAN

			// Get back buffer
			wrl::ComPtr<ID3D11RenderTargetView> rtv;
		ID3D11Texture2D* pBackBuffer = nullptr;
		GFX_THROW_INFO(swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
		GFX_THROW_INFO(GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, rtv.GetAddressOf()));
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
		GFX_THROW_INFO(GetDevice()->CreateTexture2D(&descDepth, NULL, pDepthStencil.GetAddressOf()));

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		GFX_THROW_INFO(GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, m_dsv.GetAddressOf()));

		m_width = rtDesc.Width;
		m_height = rtDesc.Height;
	}

	RenderTarget::~RenderTarget() {
		/*for (wrl::ComPtr<ID3D11RenderTargetView> rtv : m_rtv) {
			rtv->Release();
		}
		for (Texture2D* texture : m_textures) {
			delete(texture);
		}
		m_dsv->Release();*/
	}

	void RenderTarget::Setup() {


		INFOMAN

			// Depth-Stencil Texture
			wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = m_width;
		descDepth.Height = m_height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		GFX_THROW_INFO(GetDevice()->CreateTexture2D(&descDepth, NULL, pDepthStencil.GetAddressOf()));

		// Depth Stencil View 
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		GFX_THROW_INFO(GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, m_dsv.GetAddressOf()));

		// Texture description
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = m_width;
		texDesc.Height = m_height;
		texDesc.Format = m_format;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		assert(m_count < 10 && "RenderTarget: texture count is higher than 10. Did you initialize m_count?");

		for (int i = 0; i < m_count; i++) {

			// Create Texture 2D
			wrl::ComPtr<ID3D11Texture2D> pTexture;
			GFX_THROW_INFO(GetDevice()->CreateTexture2D(&texDesc, NULL, pTexture.GetAddressOf()));

			// Create Render Target View
			wrl::ComPtr<ID3D11RenderTargetView> rtv;
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = m_format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			GFX_THROW_INFO(GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, rtv.GetAddressOf()));

			m_rtv.push_back(rtv);

			m_textures.push_back(memory::Factory::Create<Texture2D>(pTexture, m_format, m_slot + i));
		}
		m_textures.push_back(memory::Factory::Create<Texture2D>(pDepthStencil, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, m_slot + m_count));
	}

	void RenderTarget::Bind() const {


		//ID3D11RenderTargetView* const * srv = (m_rtv.size() ? m_rtv[0].GetAddressOf() : nullptr);
		if (m_rtv.size()) {
			GetContext()->OMSetRenderTargets(m_rtv.size(), m_rtv[0].GetAddressOf(), m_dsv.Get());
		}
		else {
			GetContext()->OMSetRenderTargets(0, nullptr, m_dsv.Get());
		}
	}

	void RenderTarget::Unbind() const {

		if (m_rtv.size()) {
			std::vector<ID3D11RenderTargetView*> nullViews(m_rtv.size(), nullptr);
			GetContext()->OMSetRenderTargets(m_rtv.size(), nullViews.data(), nullptr);
		}
		else {
			ID3D11RenderTargetView* nullviews[1] = { nullptr };
			GetContext()->OMSetRenderTargets(1, nullviews, nullptr);
		}
	}

	void RenderTarget::Clear(float r, float g, float b) const {

		INFOMAN_NOHR
			const float m_color[] = { r, g, b, 1.0f };
		for (wrl::ComPtr<ID3D11RenderTargetView> rtv : m_rtv) {
			GFX_THROW_INFO_ONLY(GetContext()->ClearRenderTargetView(rtv.Get(), m_color));
		}
		GFX_THROW_INFO_ONLY(GetContext()->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0u));
	}	

		REFLECT_STRUCT_BEGIN(RenderTarget, StateBindable)
		REFLECT_STRUCT_MEMBER(m_count)
		REFLECT_STRUCT_MEMBER(m_width)
		REFLECT_STRUCT_MEMBER(m_height)
		REFLECT_STRUCT_MEMBER(m_format)
		REFLECT_STRUCT_MEMBER(m_slot)
		REFLECT_STRUCT_END(RenderTarget)
}

IMPLEMENT_REFLECTION_PRIMITIVE(DXGI_FORMAT, DXGI_FORMAT)