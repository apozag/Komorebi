#pragma once

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

#include "ConstantBuffer.h"

class Graphics;

#define MAX_BONES 100

struct BoneCBuffData {
	DirectX::XMMATRIX transforms[MAX_BONES];
};

class Skeleton {
public:
	Skeleton(Graphics& gfx);
	void UpdateBone(int idx, DirectX::XMMATRIX matrix);
	void Upload(Graphics& gfx);
	void Bind(Graphics& gfx);
	bool isDirty() { return m_dirty; }
private:
	bool m_dirty;
	BoneCBuffData m_boneCBuffData;
	VertexConstantBuffer<BoneCBuffData> m_boneCBuff;
};