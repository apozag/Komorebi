#pragma once

#include "Bindable.h"

class IndexBuffer : public Bindable {
public:
	IndexBuffer(Graphics& gfx, unsigned short* indices, unsigned int count);
	void Bind(Graphics& gfx)const override;
	void Unbind(Graphics& gfx)const override;
	void Update(Graphics& gfx);
	unsigned int GetCount() { return m_count; }
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	unsigned int m_count;
};
