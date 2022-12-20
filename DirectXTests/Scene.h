#pragma once

#include <vector>

#include "Node.h"
#include "Reflection.h"

class Graphics;
class Renderer;
class Entity;

class Scene {
public:
	Scene() = default;
	~Scene() = default;
	void LoadScene( );

	void Traverse();

	Node* AddNode(Entity* entity, const Transform& transform, Node* m_parent = nullptr, bool serializable = true);
	Node* AddNode(std::vector<Entity*> m_entities, const Transform& transform, Node* m_parent = nullptr, bool serializable = true);

	Node* GetMainCameraNode() { return m_mainCamera; }

	Node* GetRootNode() { return &m_transformHierarchy; }

	void Serialize(const char* filename);

private:

	void TraverseNode(Node* node, bool dirty);

private:
	Node* m_mainCamera;
	Node m_transformHierarchy;
};

