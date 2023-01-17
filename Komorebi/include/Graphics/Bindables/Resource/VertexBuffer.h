#pragma once

#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Core/Defines.h"

class VertexBuffer : public ResourceBindable{
public:
	VertexBuffer(POD::Vertex* vertices, unsigned int size, unsigned int stride, unsigned int offset);
	~VertexBuffer();
	void Bind()const override;
	void Unbind()const override;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	unsigned int m_stride, m_offset;
};