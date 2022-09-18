#pragma once

#include <vector>

#include "Node.h"

class Graphics;
class Renderer;
class Entity;

class Scene {
public:
	Scene() = default;
	~Scene() = default;
	void LoadScene( );

	void Traverse();

	Node* AddNode(Entity* entity, const Transform& transform, Node* parent = nullptr);
	Node* AddNode(std::vector<Entity*> entities, const Transform& transform, Node* parent = nullptr);

	Node* GetMainCameraNode() { return m_mainCamera; }

	Node* GetRootNode() { return &m_transformHierarchy; }
private:
	void TraverseNode(Node* node, bool dirty);

private:
	Node* m_mainCamera;
	Node m_transformHierarchy;
};

