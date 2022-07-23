#include "SkinnedVertexBuffer.h"
#include "GraphicsThrowMacros.h"
#include "Defines.h"

namespace wrl = Microsoft::WRL;

SkinnedVertexBuffer::SkinnedVertexBuffer(Graphics& gfx, POD::SkinnedVertex* data, unsigned int size, unsigned int stride, unsigned int offset) {
	INFOMAN(gfx);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = size * sizeof(POD::SkinnedVertex);
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = data;

	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &m_vertexBuffer));

	m_stride = stride;
	m_offset = offset;
}
void SkinnedVertexBuffer::Bind(Graphics& gfx) const {
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
}
void SkinnedVertexBuffer::Unbind(Graphics& gfx) const {
}
void SkinnedVertexBuffer::Update(Graphics& gfx) {

}