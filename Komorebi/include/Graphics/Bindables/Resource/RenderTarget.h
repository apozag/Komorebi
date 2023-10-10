#pragma once

#include <unordered_map>

#include "Graphics/Bindables/State/StateBindable.h"
#include "Core/Reflection/ReflectionMacros.h"

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct IDXGISwapChain;
enum DXGI_FORMAT;

namespace gfx {

	class Texture2D;
	class Viewport;

	class RenderTarget : public StateBindable {
	public:
		RenderTarget(IDXGISwapChain* m_swapChain);
		RenderTarget(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int count, unsigned int slot, unsigned int mipLevels = 1u) :
			m_width(width), m_height(height), m_format(format), m_count(count), m_slot(slot), m_mipLevels(mipLevels){
		};
		RenderTarget() {};
		~RenderTarget();

		virtual void Setup() override;

		void Bind()const override;
		void Unbind()const override;

		void BindWithDS(const RenderTarget* rt) const;

		void Update() {};

		void Clear(float r, float g, float b, float a = 1.f) const;

		unsigned int GetWidth() { return m_width; }
		unsigned int GetHeight() { return m_height; }

		const std::vector<gfx::Texture2D*>& GetTextures2D() const { return m_textures; }

		REFLECT_BASE()

	protected:
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_rtv;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
		std::vector <gfx::Texture2D* > m_textures;
		Viewport* m_viewport = nullptr;

		/////////////////////////////////////////////////////
		// Serializable
		/////////////////////////////////////////////////////
	protected:
		int m_width, m_height;
		DXGI_FORMAT m_format;
		unsigned int m_count, m_slot;
		unsigned int m_mipLevels = 1u;
	};
}

DECLARE_REFLECTION_ENUM(DXGI_FORMAT)
