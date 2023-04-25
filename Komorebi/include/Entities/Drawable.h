#pragma once

#include <vector>
#include "Entities/Entity.h"
//#include "Core/Math/MathWrappers.h"
#include "Core/Math/BvhData.h"

namespace gfx {
	class Graphics;
	class Pass;
	class ResourceBindable;
	class IndexBuffer;
	class MaterialInstance;

	template<typename T>
	class VertexConstantBuffer;
}

class Drawable : public Entity {
	friend class Renderer;
private:
	struct ModelMatrixData {
		DirectX::XMMATRIX model;
	};
public:
	
public:
	Drawable() {};
	Drawable(const Drawable& drawable);

	~Drawable();

	Drawable* Clone();

	virtual void Setup() override;

	void AddBindable(gfx::ResourceBindable* bind);
	void AddIndexBuffer(gfx::IndexBuffer* ib);
	const BVHData& GetBVHData() const { return m_bvhData; }
	
	void Insert(Node* node, const Transform& worldTransform) override;

	virtual void Draw(const DirectX::XMMATRIX&& modelMatrix) const;

	REFLECT()

public:
	gfx::MaterialInstance* m_material;
protected:
	BVHData m_bvhData;
private:
	std::vector<gfx::ResourceBindable*> m_binds;
	unsigned int m_indexCount;

	gfx::VertexConstantBuffer<ModelMatrixData>* m_modelCbuffer;
};