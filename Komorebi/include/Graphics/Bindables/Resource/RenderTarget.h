#pragma once

#include <unordered_map>

#include "Graphics/Bindables/State/StateBindable.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Core/Reflection/ReflectionMacros.h"

class RenderTarget : public StateBindable{
public:
	RenderTarget(IDXGISwapChain* m_swapChain);
	RenderTarget(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int count, unsigned int slot) :
		m_width(width), m_height(height),	m_format(format),	m_count(count), m_slot(slot)
	{};
	RenderTarget() {};
	~RenderTarget();

	virtual void Setup() override;

	void Bind()const override;
	void Unbind()const override;

	void Update() {};

	void Clear(float r, float g, float b);

	unsigned int GetWidth() { return m_width; }
	unsigned int GetHeight() { return m_height; }

	const std::vector<Texture2D*>& GetTextures2D() { return m_textures; }

	REFLECT_BASE()

private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	std::vector<Texture2D*> m_textures;

/////////////////////////////////////////////////////
// Serializable
/////////////////////////////////////////////////////
private:
	unsigned int m_width, m_height;
	DXGI_FORMAT m_format;
	unsigned int m_count, m_slot;
};

DECLARE_REFLECTION_PRIMITIVE(DXGI_FORMAT)
