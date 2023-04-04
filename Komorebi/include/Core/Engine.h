#pragma once

#include <vector>


namespace gfx {
	class RenderTarget;
	class Renderer;
	class Bindable;
}

class Scene;
class ScriptDispatcher;
class Window;

class Engine {
	friend class gfx::Bindable;
	friend class Entity;

	typedef void (*CallbackFunc)(float);

public:

	Engine() = delete;

	static void Init(const char* windowTitle, int windowWidth, int windowHeight, float targetFrameRate);
	static int Run();

	static void AddPreRenderCallback(CallbackFunc func) { m_preRenderCallbacks.push_back(func); }
	static void AddPostRenderCallback(CallbackFunc func) { m_postRenderCallbacks.push_back(func); }

	static float GetDeltaTime() { return m_dt; }
	static gfx::RenderTarget* GetDefaultRendertarget();

	static Window* GetWindow() { return m_window; }
	static gfx::Renderer* GetRenderer() { return m_renderer; }

	static Scene* m_activeScene;

private:

	static gfx::Renderer* m_renderer;
	static Window* m_window;

	static ScriptDispatcher m_scriptDispatcher;

	static std::vector<CallbackFunc> m_preRenderCallbacks;
	static std::vector<CallbackFunc> m_postRenderCallbacks;

	static float m_dt;

	static float m_targetFramerate;
};