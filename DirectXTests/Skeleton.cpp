#include "Skeleton.h"

#include "Renderer.h"

Skeleton::Skeleton(Graphics& gfx) : m_boneCBuff(gfx, CBuffSlots::BONES_CBUFF_SLOT){}

void Skeleton::UpdateBone(int idx, DirectX::XMMATRIX matrix){
	m_dirty = true;
	m_boneCBuffData.transforms[idx] = DirectX::XMMatrixTranspose(matrix);
}
void Skeleton::Upload(Graphics& gfx){
	m_boneCBuff.SetBuffer(m_boneCBuffData);
	m_boneCBuff.Update(gfx);
	m_dirty = false;
}
void Skeleton::Bind(Graphics& gfx){
	m_boneCBuff.Bind(gfx);
}