#include "VertexBuffer.h"
#include "GraphicsThrowMacros.h"
#include "Defines.h"

namespace wrl = Microsoft::WRL;

VertexBuffer::VertexBuffer( POD::Vertex* data, unsigned int size, unsigned int stride, unsigned int offset){
	INFOMAN;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = size * sizeof(POD::Vertex);
	bd.StructureByteStride = 0;// sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = data;

	GFX_THROW_INFO(GetDevice ()->CreateBuffer(&bd, &sd, &m_vertexBuffer));

	m_stride = stride;
	m_offset = offset;
}

VertexBuffer::~VertexBuffer() {
	m_vertexBuffer->Release();
}

void VertexBuffer::Bind( ) const {
	GetContext ()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
}
void VertexBuffer::Unbind( ) const {
}
void VertexBuffer::Update( ) {

}