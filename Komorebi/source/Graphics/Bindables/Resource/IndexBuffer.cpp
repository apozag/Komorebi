#include "Graphics/Bindables/Resource/IndexBuffer.h"
#include "Graphics/GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

IndexBuffer::IndexBuffer(unsigned short* indices, unsigned int count) {
	INFOMAN;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = count * sizeof(unsigned short);
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	isd.SysMemPitch = 0;
	isd.SysMemSlicePitch = 0;
	GFX_THROW_INFO(GetDevice()->CreateBuffer(&ibd, &isd, m_indexBuffer.GetAddressOf()));
	m_count = count;
}

IndexBuffer::~IndexBuffer() {
	m_indexBuffer->Release();
}

void IndexBuffer::Bind() const {
	GetContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}
void IndexBuffer::Unbind() const {
}
