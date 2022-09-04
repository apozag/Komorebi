#include "ResourceBindable.h"
#include "Renderer.h"
#include "IndexBuffer.h"
#include "Pass.h"

#include "Drawable.h"

Drawable::Drawable() {
	m_modelCbuffer = new VertexConstantBuffer<ModelMatrixData>  ( VCBUFF_MODEL_SLOT);
	AddBindable(m_modelCbuffer);
}

Drawable::Drawable(const Drawable& drawable) {
	this->m_indexCount = drawable.m_indexCount;
	this->m_binds = std::vector(drawable.m_binds);
	this->m_material = drawable.m_material;
	this->m_modelCbuffer = drawable.m_modelCbuffer;
	this->m_bvhData = drawable.m_bvhData;
}

Drawable* Drawable::Clone() {
	return new Drawable(*this);
}

void Drawable::AddBindable(ResourceBindable* bindable) {
	m_binds.push_back(bindable);
}

void Drawable::AddIndexBuffer(IndexBuffer* ib) {
	m_indexCount = ib->GetCount();
	m_binds.push_back(ib);
}

void Drawable::Insert(Node* node, const Transform& worldTransform) {
	GetRenderer()->SubmitDrawable(this, &worldTransform, m_material);
}

void Drawable::Draw(DirectX::XMMATRIX&& modelMatrix) const {
	for (ResourceBindable* bind : m_binds) {
		bind->Update ();
	}

	for (ResourceBindable* bind : m_binds) {
		bind->Bind ();
	}

	m_modelCbuffer->SetBuffer({ modelMatrix });
	
	GetGraphics()->DrawIndexed(m_indexCount);

	for (ResourceBindable* bind : m_binds) {
		bind->Unbind ();
	}
}