#include "Graphics/Bindables/Resource/SkinnedVertexBuffer.h"
#include "Graphics/GraphicsThrowMacros.h"
#include "Core/Defines.h"

namespace wrl = Microsoft::WRL;

SkinnedVertexBuffer::SkinnedVertexBuffer( POD::SkinnedVertex* data, unsigned int size, unsigned int stride, unsigned int offset) {
	INFOMAN;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = size * sizeof(POD::SkinnedVertex);
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = data;

	GFX_THROW_INFO(GetDevice ()->CreateBuffer(&bd, &sd, &m_vertexBuffer));

	m_stride = stride;
	m_offset = offset;
}

SkinnedVertexBuffer::~SkinnedVertexBuffer() {
	m_vertexBuffer->Release();
}

void SkinnedVertexBuffer::Bind( ) const {
	GetContext ()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
}
void SkinnedVertexBuffer::Unbind( ) const {
}
void SkinnedVertexBuffer::Update( ) {

}