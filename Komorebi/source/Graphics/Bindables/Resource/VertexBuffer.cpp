#include "Graphics/Bindables/Resource/VertexBuffer.h"
#include "Graphics/GraphicsThrowMacros.h"
#include "Core/Defines.h"

namespace wrl = Microsoft::WRL;

namespace gfx {

	VertexBuffer::VertexBuffer(void* data, unsigned int size, unsigned int stride, unsigned int offset, bool dynamic) {
		INFOMAN

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = size;
		bd.StructureByteStride = 0;// sizeof(Vertex);

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = data;

		GFX_THROW_INFO(GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer));

		m_stride = stride;
		m_offset = offset;
		m_count = (float)(size - offset) / (float)stride;
	}

	VertexBuffer::~VertexBuffer() {
		//m_vertexBuffer->Release();
	}

	void VertexBuffer::Bind() const {
		GetContext()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
	}
	void VertexBuffer::Unbind() const {
	}
	void VertexBuffer::Update() {

	}
}
