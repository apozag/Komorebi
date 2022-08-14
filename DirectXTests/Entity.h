#pragma once

#include "Transform.h"
#include "Engine.h"
#include "Window.h"

class Renderer;
class ScriptDispatcher;

class Entity {
public:
	virtual void Insert(Node* node, const Transform& worldTransform) = 0;
protected:
	Renderer* GetRenderer() const { return Engine::m_renderer; }
	ScriptDispatcher* GetScriptDispatcher() const { return &Engine::m_scriptDispatcher; }
	const Graphics* GetGraphics() const { return Engine::m_window->gfx; }
	const Window* GetWindow() const { return Engine::m_window; }
};