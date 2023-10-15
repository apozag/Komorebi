#pragma once

#include <vector>
#include "Entities/Entity.h"
#include "Core/Math/BvhData.h"
#include "Graphics/Topology.h"
#include "Graphics/MaterialInstance.h"

namespace gfx {
	class Graphics;
	class Pass;
	class ResourceBindable;
	class IndexBuffer;	

	template<typename T>
	class ConstantBuffer;
	class VertexBuffer;
}

class Drawable : public Entity {
	friend class Renderer;
private:
	struct ModelMatrixData {
		DirectX::XMMATRIX model;
	};
	
public:
	Drawable() {};
	Drawable(const Drawable& drawable);

	~Drawable();

	Drawable* Clone();

	virtual void Setup() override;

	void AddBindable(gfx::ResourceBindable* bind);
	void AddBindable(gfx::VertexBuffer* bind);
	void AddIndexBuffer(gfx::IndexBuffer* ib);
	const BVHData& GetBVHData() const { return m_bvhData; }
	
	void Insert(Node* node, const Transform& worldTransform) override;

	virtual void Draw(const DirectX::XMMATRIX&& modelMatrix) const;

	void SetVertexCount(unsigned int count) { m_vertexCount = count; }
	void SetIndexCount(unsigned int count) { m_indexCount = count; }

	void CopyFromMaterialInstance(const gfx::MaterialInstance*);

	REFLECT()

public:
	gfx::MaterialInstance m_matInstance;

	gfx::Topology m_topology = gfx::Topology::TRIANGLES;

protected:
	BVHData m_bvhData;
private:
	std::vector<gfx::ResourceBindable*> m_binds;
	unsigned int m_indexCount;
	unsigned int m_vertexCount;

	gfx::ConstantBuffer<ModelMatrixData>* m_modelCbuffer;

	bool m_bIndexed = false;
	
};

