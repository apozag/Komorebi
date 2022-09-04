#pragma once

#include "ResourceBindable.h"
#include "Defines.h"

class SkinnedVertexBuffer : public ResourceBindable {
public:
	SkinnedVertexBuffer(POD::SkinnedVertex* vertices, unsigned int size, unsigned int stride, unsigned int offset);
	~SkinnedVertexBuffer();
	void Bind()const override;
	void Unbind()const override;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	unsigned int m_stride, m_offset;
};