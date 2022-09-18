#include "Skeleton.h"

#include "Renderer.h"

Skeleton::Skeleton() : m_boneCBuff ( VCBUFF_BONES_SLOT){}

void Skeleton::UpdateBone(int idx, DirectX::XMMATRIX matrix){
	m_dirty = true;
	m_boneCBuffData.transforms[idx] = DirectX::XMMatrixTranspose(matrix);
}
void Skeleton::Upload( ){
	m_boneCBuff.SetBuffer(m_boneCBuffData);
	m_boneCBuff.Update ();
	m_dirty = false;
}
void Skeleton::Bind( ){
	m_boneCBuff.Bind ();
}