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
private:
	Transform globalTransform;
};
