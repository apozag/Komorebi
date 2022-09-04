#pragma once

#include <unordered_map>

#include "StateBindable.h"
#include "Texture2D.h"

class RenderTarget : public StateBindable{
public:
	RenderTarget(IDXGISwapChain* m_swapChain);
	RenderTarget( int width, int height, DXGI_FORMAT format, int count, int slot);
	~RenderTarget();

	void Bind()const override;
	void Unbind()const override;

	void Update() {};

	void Clear(float r, float g, float b);

	unsigned int GetWidth() { return m_width; }
	unsigned int GetHeight() { return m_height; }

	const std::vector<Texture2D*>& GetTextures2D() { return m_textures; }

private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	std::vector<Texture2D*> m_textures;

	unsigned int m_width, m_height;
};
