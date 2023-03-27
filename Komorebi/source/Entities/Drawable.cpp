#define NOMINMAX

#include "Core/Memory/Factory.h"
#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Graphics/Renderer.h"
#include "Graphics/Bindables/Resource/IndexBuffer.h"
#include "Graphics/Pass.h"

#include "Entities/Drawable.h"

Drawable::Drawable(const Drawable& drawable) {
	this->m_indexCount = drawable.m_indexCount;
	this->m_binds = std::vector(drawable.m_binds);
	this->m_material = drawable.m_material;
	this->m_modelCbuffer = drawable.m_modelCbuffer;
	this->m_bvhData = drawable.m_bvhData;
}

Drawable* Drawable::Clone() {
	return memory::Factory::Create<Drawable>(*this);//new Drawable(*this);
}

void Drawable::Setup() {
	m_modelCbuffer = memory::Factory::Create< VertexConstantBuffer<ModelMatrixData>>(VCBUFF_MODEL_SLOT);
	AddBindable(m_modelCbuffer);
}

Drawable::~Drawable() {
	//delete(m_modelCbuffer);
	for (ResourceBindable* bind : m_binds) {
		//delete(bind);
	}
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

void Drawable::Draw(const DirectX::XMMATRIX& modelMatrix) const {

	m_modelCbuffer->SetBuffer({ modelMatrix});

	for (ResourceBindable* bind : m_binds) {
		bind->Update ();
	}

	for (ResourceBindable* bind : m_binds) {
		bind->Bind ();
	}	
	
	GetGraphics()->DrawIndexed(m_indexCount);

	for (ResourceBindable* bind : m_binds) {
		bind->Unbind ();
	}
}


typedef Drawable::BVHData bvhDataType;
REFLECT_STRUCT_BASE_BEGIN(bvhDataType)
REFLECT_STRUCT_MEMBER(m_min)
REFLECT_STRUCT_MEMBER(m_max)
REFLECT_STRUCT_END(bvhDataType)

REFLECT_STRUCT_BEGIN(Drawable, Entity)
REFLECT_STRUCT_MEMBER(m_material)
REFLECT_STRUCT_MEMBER(m_bvhData)
REFLECT_STRUCT_END(Drawable)
