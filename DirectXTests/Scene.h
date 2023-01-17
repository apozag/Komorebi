#pragma once

#include <vector>

#include "Node.h"
#include "ReflectionMacros.h"

class Graphics;
class Renderer;
class Entity;

class Scene : public GameObject{
	friend class SceneLoader;
public:
	Scene() = default;
	~Scene() = default;
	void LoadScene( );

	void Traverse();

	void Setup() {}

	Node* AddNode(Entity* entity, const Transform& transform, Node* m_parent = nullptr);
	Node* AddNode(std::vector<Entity*> m_entities, const Transform& transform, Node* m_parent = nullptr);

	//Node* GetMainCameraNode() { return m_mainCamera; }

	Node* GetRootNode() { return m_transformHierarchy; }

	void Serialize(const char* filename);

	REFLECT_BASE()

	OWNED_PTR(Node) m_transformHierarchy;

private:

	void TraverseNode(Node* node, bool dirty);

};

