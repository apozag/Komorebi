#define NOMINMAX

#include "Entities/Drawable.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Memory/Factory.h"
#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Graphics/Renderer.h"
#include "Graphics/Bindables/Resource/IndexBuffer.h"
#include "Graphics/Bindables/Resource/VertexBuffer.h"
#include "Graphics/Pass.h"
#include "Graphics/Material.h"

Drawable::Drawable(const Drawable& drawable) {
	this->m_indexCount = drawable.m_indexCount;
	this->m_binds = std::vector(drawable.m_binds);
	this->m_matInstance = drawable.m_matInstance;
	this->m_modelCbuffer = drawable.m_modelCbuffer;
	this->m_bvhData = drawable.m_bvhData;
}

Drawable* Drawable::Clone() {
	return memory::Factory::Create<Drawable>(*this);//new Drawable(*this);
}

void Drawable::Setup() {
	m_modelCbuffer = memory::Factory::Create< gfx::ConstantBuffer<ModelMatrixData>>(VCBUFF_MODEL_SLOT, true, nullptr, (unsigned int)gfx::CBufferStage::VERTEX);
	AddBindable(m_modelCbuffer);
}

Drawable::~Drawable() {	
	for (gfx::ResourceBindable* bind : m_binds) {
		memory::Factory::Destroy(bind);
	}
}

void Drawable::AddBindable(gfx::ResourceBindable* bindable) {
	m_binds.push_back(bindable);
}

void Drawable::AddBindable(gfx::VertexBuffer* vb) {
	m_vertexCount = vb->GetCount();
	m_binds.push_back(vb);
}

void Drawable::AddIndexBuffer(gfx::IndexBuffer* ib) {
	m_indexCount = ib->GetCount();
	m_binds.push_back(ib);
	m_bIndexed = true;
}

void Drawable::Insert(Node* node, const Transform& worldTransform) {
	GetRenderer()->SubmitDrawable(this, &worldTransform, m_matInstance);
}

void Drawable::Draw(const DirectX::XMMATRIX&& modelMatrix) const {

	m_modelCbuffer->m_buffer = { modelMatrix };

	for (gfx::ResourceBindable* bind : m_binds) {
		bind->Update();
		bind->Bind ();
	}	

	GetGraphics()->SetTopology(m_topology);
	
	if (m_bIndexed) {
		GetGraphics()->DrawIndexed(m_indexCount);
	}
	else {
		GetGraphics()->Draw(m_vertexCount);
	}

	for (gfx::ResourceBindable* bind : m_binds) {
		bind->Unbind ();
	}
}

REFLECT_STRUCT_BEGIN(Drawable, Entity)
REFLECT_STRUCT_MEMBER(m_matInstance)
REFLECT_STRUCT_MEMBER(m_topology)
REFLECT_STRUCT_MEMBER(m_bvhData)
REFLECT_STRUCT_END(Drawable)
