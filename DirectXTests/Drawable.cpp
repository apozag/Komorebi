#include "Bindable.h"
#include "Renderer.h"
#include "IndexBuffer.h"
#include "Pass.h"

#include "Drawable.h"

Drawable::Drawable(Graphics& gfx) {
	m_modelCbuffer = new VertexConstantBuffer<ModelMatrixData> (gfx, CBuffSlots::MODEL_CBUFF_SLOT);
	AddBindable(m_modelCbuffer);
}

Drawable::Drawable(const Drawable& drawable) {
	this->m_indexCount = drawable.m_indexCount;
	this->m_binds = std::vector(drawable.m_binds);
	this->m_passes = std::vector(drawable.m_passes);
	this->m_modelCbuffer = drawable.m_modelCbuffer;
	this->m_bvhData = drawable.m_bvhData;
}

Drawable* Drawable::Clone() {
	return new Drawable(*this);
}

void Drawable::AddBindable(Bindable* bindable) {
	m_binds.push_back(bindable);
}

void Drawable::AddIndexBuffer(IndexBuffer* ib) {
	m_indexCount = ib->GetCount();
	m_binds.push_back(ib);
}

void Drawable::AddPass(Pass* pass) {
	m_passes.push_back(pass);
}

void Drawable::Insert(Renderer& renderer, const Transform& worldTransform) {
	renderer.SubmitDrawable(this, &worldTransform, m_passes);
}

void Drawable::Draw(Graphics& gfx) const {
	for (Bindable* bind : m_binds) {
		bind->Update(gfx);
	}

	for (Bindable* bind : m_binds) {
		bind->Bind(gfx);
	}
	gfx.DrawIndexed(m_indexCount);

	for (Bindable* bind : m_binds) {
		bind->Unbind(gfx);
	}
}