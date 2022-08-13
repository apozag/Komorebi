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

	void RenderTraverse();

	Node* AddNode(Entity* entity, const Transform& transform, Node* parent = nullptr);

	Node* GetMainCameraNode() { return m_mainCamera; }

	Node* GetRootNode() { return &m_transformHierarchy; }
private:
	void RenderTraverseNode(Node* node, bool dirty);

private:
	Node* m_mainCamera;
	Node m_transformHierarchy;
	//std::vector<Node> m_nodes;
};

