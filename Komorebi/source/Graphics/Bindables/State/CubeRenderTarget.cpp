#include "Graphics/Bindables/State/CubeRenderTarget.h"

#include "Graphics/Bindables/Resource/CubeTexture.h"
#include "Graphics/Bindables/State/Viewport.h"
#include "Graphics/GraphicsThrowMacros.h"
#include "Core/Memory/Factory.h"
#include "Core/Engine.h"
#include "Core/Window.h"

namespace wrl = Microsoft::WRL;

namespace gfx {

  void CubeRenderTarget::Setup() {
		INFOMAN

		if (m_width < 0) {
			m_width = Engine::GetWindow()->GetClientWidth();
		}
		if (m_height < 0) {
			m_height = Engine::GetWindow()->GetClientHeight();
		}

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = m_width;
		texDesc.Height = m_height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 6;
		texDesc.Format = m_format;
		texDesc.CPUAccessFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Create Texture 2D
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice()->CreateTexture2D(&texDesc, NULL, pTexture.GetAddressOf()));
		m_textures.push_back(memory::Factory::Create<CubeTexture>(pTexture, m_format, m_slot));

		// Create Render Target View
		wrl::ComPtr<ID3D11RenderTargetView> rtv;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.ArraySize = 6;
		GFX_THROW_INFO(GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, rtv.GetAddressOf()));

		m_rtv.push_back(rtv);

		if (!m_viewport) {
			m_viewport = memory::Factory::Create<Viewport>(0, 0, m_width, m_height);
			m_viewport->Setup();
		}
		
  }

  CubeTexture* CubeRenderTarget::GetCubeTexture() {
    if (m_textures.size() > 0) {
      return static_cast<CubeTexture*>(m_textures[0]);
    }
    return nullptr;
  }

}