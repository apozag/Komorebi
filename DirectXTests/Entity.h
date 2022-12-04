#pragma once

#include "Transform.h"
#include "Engine.h"
#include "Window.h"
#include "Reflection.h"

class Renderer;
class ScriptDispatcher;
class Transform;

class Entity {
public:
	virtual void Insert(Node* node, const Transform& worldTransform) = 0;
	//uint32_t GetTagMask() const{ return m_tagMask; }
	//void SetTag(const char* tag);
	REFLECT();
protected:
	Renderer* GetRenderer() const { return Engine::m_renderer; }
	ScriptDispatcher* GetScriptDispatcher() const { return &Engine::m_scriptDispatcher; }
	const Graphics* GetGraphics() const { return Engine::m_window->gfx; }
	const Window* GetWindow() const { return Engine::m_window; }
public:
	uint32_t m_tagMask = 0;
};


DECLARE_REFLECTION_PRIMITIVE(uint32_t)
DECLARE_REFLECTION_PRIMITIVE(Entity*)