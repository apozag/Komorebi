#pragma once

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

class Renderer;
class ScriptDispatcher;
class Transform;
class Node;
class Window;
class Engine;
class Graphics;

class Entity : public GameObject{
	friend class Scene;
public:
	virtual void Insert(Node* node, const Transform& worldTransform) = 0;
	REFLECT_BASE();
protected:
	Renderer* GetRenderer() const;
	ScriptDispatcher* GetScriptDispatcher() const;
	const Graphics* GetGraphics() const;
	const Window* GetWindow() const;

	Node* m_node;

public:
	uint32_t m_tag = 0;
};

DECLARE_REFLECTION_POINTER(Entity)