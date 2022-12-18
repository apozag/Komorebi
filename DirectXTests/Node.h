#pragma once

#include <vector>
#include "Transform.h"
#include "Entity.h"
#include "Reflection.h"

class Scene;

class Node {
	friend class Scene;
public:
	std::vector<Entity*> m_entities;
	Transform m_localTransform;
	std::vector<Node*> m_children;
	Node* m_parent;

	template<class T>
	inline T* GetEntity();

	REFLECT_BASE()

private:
	Transform m_globalTransform;
};

template<class T>
T* Node::GetEntity() {
	T* e = nullptr;
	for (Entity* entity : m_entities) {
		e = dynamic_cast<T*>(entity);
		if (e) return e;
	}
	return e;
}