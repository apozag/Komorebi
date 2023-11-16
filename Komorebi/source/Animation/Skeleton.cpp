#include "Animation/Skeleton.h"

#include "Graphics/Renderer.h"
#include "Graphics/PipelineStage.h"

Skeleton::Skeleton() : m_cbuffer( VCBUFF_BONES_SLOT, true, nullptr, gfx::PipelineStage::VERTEX){}

void Skeleton::UpdateBone(int idx, DirectX::XMMATRIX matrix){
	m_dirty = true;
	m_cbuffer.m_buffer.transforms[idx] = DirectX::XMMatrixTranspose(matrix);
}
void Skeleton::Upload( ){
	m_cbuffer.Update ();
	m_dirty = false;
}
void Skeleton::Bind( ){
	m_cbuffer.Bind ();
}