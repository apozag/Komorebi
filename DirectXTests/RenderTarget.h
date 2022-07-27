#pragma once

#include <unordered_map>

#include "Bindable.h"
#include "Texture2D.h"

class RenderTarget : public Bindable{
public:
	RenderTarget(Graphics& gfx, IDXGISwapChain* m_swapChain);
	RenderTarget(Graphics& gfx, int width, int height, DXGI_FORMAT format, int count, int slot);

	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;

	void Update(Graphics& gfx) {};

	void Clear(Graphics& gfx, float r, float g, float b);

	void SetAsShaderResource();
	void SetAsRenderTarget();

	void Lock();
	void Unlock();

	unsigned int GetWidth() { return m_width; }
	unsigned int GetHeight() { return m_height; }

private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_rtv;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_srv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	unsigned int m_width, m_height;
	unsigned int m_slot;
	bool isShaderResource = true;
	//bool bindDepth = false;
	bool locked = false;
	int m_id;
};
