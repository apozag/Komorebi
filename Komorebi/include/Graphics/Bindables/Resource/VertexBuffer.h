#pragma once

#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Core/Defines.h"

struct ID3D11Buffer;

namespace gfx {

	class VertexBuffer : public ResourceBindable {
	public:
		VertexBuffer(void* vertices, unsigned int size, unsigned int stride, unsigned int offset, bool dynamic = false);
		~VertexBuffer();
		void SetData(void* data, size_t size);
		void Bind()const override;
		void Unbind()const override;
		void Update();

		unsigned int GetCount() { return m_count;}

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		unsigned int m_stride, m_offset, m_count;
	};
}