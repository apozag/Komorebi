#pragma once

#include <vector>
#include "Transform.h"
#include "Entity.h"
#include "ReflectionMacros.h"

class Scene;

class Node {
	friend class Scene;
public:

	Node(bool serializable = true) : m_serializable(serializable) {}

	bool IsSerializable() { return m_serializable; }

	template<class T>
	inline T* GetEntity();

	std::vector<Entity*> m_entities;
	Transform m_localTransform;
	std::vector<Node*> m_children;
	Node* m_parent;

	REFLECT_BASE()

private:
	Transform m_globalTransform;
	bool m_serializable = true;
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

DECLARE_RELFECTION_POINTER(Node*)