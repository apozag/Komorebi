#include "Entities/Bone.h"
#include "Animation/Skeleton.h"

void Bone::Insert(Node* node, const Transform& worldTransform) {
	m_skeleton->UpdateBone(m_slot, mOffsetMatrix * worldTransform.GetMatrix());
}