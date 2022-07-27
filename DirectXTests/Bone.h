#pragma once

#include <d3d11.h>
#include "SimpleMath.h"

#include "Entity.h"

class Skeleton;

class Bone : public Entity{
public:
	Bone(Skeleton* skeleton, int idx, DirectX::XMMATRIX offsetMatrix) : m_slot(idx), m_skeleton(skeleton), mOffsetMatrix(offsetMatrix) {}
	void Insert(Renderer& renderer, const Transform& worldTransform) override;
private:
	int m_slot;
	DirectX::XMMATRIX mOffsetMatrix;
	Skeleton* m_skeleton;
};