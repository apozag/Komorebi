#pragma once

#include <vector>

#include "Scene/Node.h"
#include "Core/Reflection/ReflectionMacros.h"

class Graphics;
class Renderer;
class Entity;

class Scene : public GameObject{
public:
	Scene() = default;
	~Scene() = default;
	void LoadScene( );

	void Traverse();

	void Setup() {}

	Node* AddNode(Entity* entity, const Transform& transform, Node* m_parent = nullptr, bool auxNode = false);
	Node* AddNode(std::vector<Entity*> m_entities, const Transform& transform, Node* m_parent = nullptr, bool auxNode = false);

	Node* GetRootNode() { return m_transformHierarchy; }

	void Serialize(const char* filename);

	REFLECT_BASE()

	OWNED_PTR(Node) m_transformHierarchy;

private:

	void TraverseNode(Node* node, bool dirty);

};

