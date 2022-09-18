#pragma once

#include <vector>
#include "Transform.h"
#include "Entity.h"

class Scene;

class Node {
	friend class Scene;
public:
	std::vector<Entity*> entities;
	Transform localTransform;
	std::vector<Node*> children;
	Node* parent;

	template<class T>
	inline T* GetEntity();

private:
	Transform globalTransform;
};

template<class T>
T* Node::GetEntity() {
	T* e = nullptr;
	for (Entity* entity : entities) {
		e = dynamic_cast<T*>(entity);
		if (e) return e;
	}
	return e;
}
