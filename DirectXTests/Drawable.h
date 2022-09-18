#pragma once

#include <vector>
#include "Entity.h"

class Graphics;
class Pass;
class ResourceBindable;
class IndexBuffer;
class Material;

template<typename T>
class VertexConstantBuffer;

class Drawable : public Entity {
	friend class Renderer;
private:
	struct ModelMatrixData {
		DirectX::XMMATRIX model;
	};
public:
	struct BVHData {
		DirectX::SimpleMath::Vector3 min;
		DirectX::SimpleMath::Vector3 max;
	};
public:
	Drawable();
	Drawable(const Drawable& drawable);
	Drawable* Clone();
	void AddBindable(ResourceBindable* bind);
	void AddIndexBuffer(IndexBuffer* ib);
	const BVHData& GetBVHData() const { return m_bvhData; }
	
	void Insert(Node* node, const Transform& worldTransform) override;

	virtual void Draw(DirectX::XMMATRIX&& modelMatrix) const;

public:
	Material* m_material;
protected:
	BVHData m_bvhData;
private:
	std::vector<ResourceBindable*> m_binds;
	unsigned int m_indexCount;

	VertexConstantBuffer<ModelMatrixData>* m_modelCbuffer;
};