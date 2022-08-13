#include "Bone.h"
#include "Skeleton.h"

void Bone::Insert(const Transform& worldTransform) {
	m_skeleton->UpdateBone(m_slot, mOffsetMatrix * worldTransform.GetMatrix());
}