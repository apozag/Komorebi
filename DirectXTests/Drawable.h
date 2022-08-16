#pragma once

#include <vector>
#include "Entity.h"

class Graphics;
class Pass;
class Bindable;
class IndexBuffer;

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
	void AddBindable(Bindable* bind);
	void AddIndexBuffer(IndexBuffer* ib);
	void AddPass(Pass* pass);
	std::vector<Pass*> GetPasses() const { return m_passes; }
	const BVHData& GetBVHData() const { return m_bvhData; }
	
	void Insert(Node* node, const Transform& worldTransform) override;

	virtual void Draw(DirectX::XMMATRIX&& modelMatrix) const;

protected:
	BVHData m_bvhData;
private:
	std::vector<Pass*> m_passes;
	std::vector<Bindable*> m_binds;
	unsigned int m_indexCount;

	VertexConstantBuffer<ModelMatrixData>* m_modelCbuffer;
};