#pragma once

#include "Core/Math/Transform.h"
#include "Core/Engine.h"
#include "Core/Window.h"
#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

class Renderer;
class ScriptDispatcher;
class Transform;
class Node;

class Entity : public GameObject{
public:
	virtual void Insert(Node* node, const Transform& worldTransform) = 0;
	REFLECT_BASE();
protected:
	Renderer* GetRenderer() const { return Engine::m_renderer; }
	ScriptDispatcher* GetScriptDispatcher() const { return &Engine::m_scriptDispatcher; }
	const Graphics* GetGraphics() const { return Engine::m_window->gfx; }
	const Window* GetWindow() const { return Engine::m_window; }

public:
	uint32_t m_tagMask = 0;
};

DECLARE_REFLECTION_POINTER(Entity)