#pragma once

#include <string>

#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Graphics/PipelineStage.h"

struct Image;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;
enum DXGI_FORMAT;

namespace gfx {

	class Texture2D : public ResourceBindable {
	public:
		Texture2D() {}
		Texture2D(std::string filename, unsigned int slot, unsigned int mipLevels = 1u) : ResourceBindable(slot), m_filename(filename), m_mipLevels(mipLevels) {}
		Texture2D(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels, unsigned int slot, unsigned int mipLevels = 1u);
		Texture2D(Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, DXGI_FORMAT format, int width, int height, unsigned int slot, unsigned int mipLevels = 1u);
		~Texture2D();

		virtual void Setup() override;

		void Bind() const override;
		void BindAt(unsigned int slot, int mipLevel = -1, unsigned int stageMask = PipelineStage::PIXEL) const;
		void Unbind() const override;

		void UnbindAt(unsigned int slot, unsigned int stageMask = PipelineStage::PIXEL) const;
		void BindAsUAV(unsigned int slot, unsigned int mipSlice) const;
		void UnbindUAV(unsigned int slot) const;

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		ID3D11ShaderResourceView* GetSRV() const { return (ID3D11ShaderResourceView*)m_srv.Get(); }

		void GenMipMaps();

		REFLECT()

	private:
		void CreateFromImage(const Image& image);

	protected:

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;	
		DXGI_FORMAT m_format;

		int m_width, m_height;

		std::string m_filename;

		unsigned int m_mipLevels = 1u;
	};
}