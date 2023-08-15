#pragma once

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

#include "Graphics/Bindables/Resource/ConstantBuffer.h"

//class Graphics;

#define MAX_BONES 200

struct BoneCBuffData {
	DirectX::XMMATRIX transforms[MAX_BONES];
};

class Skeleton {
public:
	Skeleton( );
	void UpdateBone(int idx, DirectX::XMMATRIX matrix);
	void Upload( );
	void Bind( );
	bool isDirty() { return m_dirty; }
private:
	bool m_dirty;
	gfx::VertexConstantBuffer<BoneCBuffData> m_cbuffer;
};