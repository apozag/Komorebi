#pragma once

#include "Graphics/Bindables/Resource/ResourceBindable.h"

struct ID3D11Buffer;

namespace gfx {

	class IndexBuffer : public ResourceBindable {
	public:
		IndexBuffer(unsigned short* indices, unsigned int count);
		~IndexBuffer();
		void Bind()const override;
		void Unbind()const override;
		unsigned int GetCount() { return m_count; }
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		unsigned int m_count;
	};
}
