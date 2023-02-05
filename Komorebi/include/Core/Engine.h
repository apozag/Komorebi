#pragma once

#include "Core/Window.h"

class Scene;
class Renderer;
class ScriptDispatcher;

class Engine {
	friend class Bindable;
	friend class Entity;

	typedef void (*CallbackFunc)(float);

public:

	Engine() = delete;

	static void Init(const char* windowTitle, int windowWidth, int windowHeight, float targetFrameRate);
	static int Run();

	static void AddPreRenderCallback(CallbackFunc func) { m_preRenderCallbacks.push_back(func); }
	static void AddPostRenderCallback(CallbackFunc func) { m_postRenderCallbacks.push_back(func); }

	static float GetDeltaTime() { return m_dt; }
	static RenderTarget* GetDefaultRendertarget() { 
		return m_window->m_gfx->GetDefaultRenderTarget(); 
	}

	static Window* GetWindow() { return m_window; }

	static Scene* m_activeScene;

private:

	static Renderer* m_renderer;
	static Window* m_window;

	static ScriptDispatcher m_scriptDispatcher;

	static std::vector<CallbackFunc> m_preRenderCallbacks;
	static std::vector<CallbackFunc> m_postRenderCallbacks;

	static float m_dt;

	static float m_targetFramerate;
};