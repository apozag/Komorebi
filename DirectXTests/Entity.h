#pragma once

#include "Transform.h"

class Renderer;
class Node;

class Entity {
	friend class Node;
public:
	virtual void Insert(Renderer& renderer, const Transform& worldTransform) = 0;
};