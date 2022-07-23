#include "Bone.h"
#include "Skeleton.h"

void Bone::Insert(Renderer& renderer, const Transform& worldTransform) {
	m_skeleton->UpdateBone(m_idx, mOffsetMatrix * worldTransform.GetMatrix());
}