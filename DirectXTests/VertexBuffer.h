#pragma once

#include "Bindable.h"
#include "Defines.h"

class VertexBuffer : public Bindable{
public:
	VertexBuffer(Graphics& gfx, POD::Vertex* vertices, unsigned int size, unsigned int stride, unsigned int offset);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
	void Update(Graphics& gfx);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	unsigned int m_stride, m_offset;
};