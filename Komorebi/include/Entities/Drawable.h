#pragma once

#include <vector>
#include "Entities/Entity.h"

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
	struct BVHData : public GameObject{

		BVHData() {}

		BVHData(const math::Vector3& min, const math::Vector3& max) : m_min(min), m_max(max) {}

		math::Vector3 m_min;
		math::Vector3 m_max;

		REFLECT_BASE()
	};
public:
	Drawable() {};
	Drawable(const Drawable& drawable);
	Drawable* Clone();

	virtual void Setup() override;

	void AddBindable(ResourceBindable* bind);
	void AddIndexBuffer(IndexBuffer* ib);
	const BVHData& GetBVHData() const { return m_bvhData; }
	
	void Insert(Node* node, const Transform& worldTransform) override;

	virtual void Draw(const DirectX::XMMATRIX& modelMatrix) const;

	REFLECT()

public:
	Material* m_material;
protected:
	BVHData m_bvhData;
private:
	std::vector<ResourceBindable*> m_binds;
	unsigned int m_indexCount;

	VertexConstantBuffer<ModelMatrixData>* m_modelCbuffer;
};