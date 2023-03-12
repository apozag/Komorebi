#pragma once

#include <vector>
#include <string>
#include "Core/Math/Transform.h"
#include "Core/Reflection/ReflectionMacros.h"

class Scene;
class Entity;

class Node : public GameObject{
	friend class Scene;
public:

	Node() : m_name("Node"){}

	template<class T>
	inline T* GetEntity();

	std::vector<OWNED_PTR(Entity)> m_entities;
	Transform m_localTransform;
	std::vector<OWNED_PTR(Node)> m_children;	

	bool m_enabled = true;

	std::string m_name;

	REFLECT_BASE()

private:
	Transform m_globalTransform;
	Node* m_parent;
};

template<class T>
T* Node::GetEntity() {
	T* e = nullptr;
	for (Entity* entity : m_entities) {
		e = dynamic_cast<T*>(entity);
		if (e) return e;
	}
	return nullptr;
}

DECLARE_REFLECTION_POINTER(Node)