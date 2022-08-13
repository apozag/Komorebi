#pragma once

#include "Window.h"

class Scene;
class Renderer;
class ScriptDispatcher;

class Engine {
	friend class Bindable;
	friend class Entity;
public:

	Engine() = delete;

	static void Init(const char* windowTitle, int windowWidth, int windowHeight, float targetFrameRate);
	static int Start();

	static float GetDeltaTime() { return m_dt; }
	static RenderTarget* GetDefaultRendertarget() { return m_window->gfx->GetDefaultRenderTarget(); }

	static Scene* m_activeScene;

private:

	static Renderer* m_renderer;
	static Window* m_window;

	static ScriptDispatcher m_scriptDispatcher;

	static float m_dt;

	static float m_targetFramerate;
};