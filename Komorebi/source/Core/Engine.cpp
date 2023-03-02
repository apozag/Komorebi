#include <thread>

#include "Core/Engine.h"
#include "Core/Time/Timer.h"
#include "Graphics/Renderer.h"
#include "Scene/Scene.h"
#include "Core/Window.h"
#include "Entities/Script.h"
#include "Core/ScriptDispatcher.h"
#include "Core/Window.h"

Renderer* Engine::m_renderer;
Window* Engine::m_window;
Scene* Engine::m_activeScene;
ScriptDispatcher Engine::m_scriptDispatcher;
std::vector<Engine::CallbackFunc> Engine::m_preRenderCallbacks;
std::vector<Engine::CallbackFunc> Engine::m_postRenderCallbacks;
float Engine::m_dt;
float Engine::m_targetFramerate;

RenderTarget* Engine::GetDefaultRendertarget() {
	return m_window->m_gfx->GetDefaultRenderTarget();
}

void Engine::Init(const char* windowTitle, int windowWidth, int windowHeight, float targetFramerate) {
	m_window = new Window(windowWidth, windowHeight, windowTitle);	
	m_window->Init();
	m_renderer = new Renderer();
	m_targetFramerate = targetFramerate;
}

int Engine::Run() 	
{	
	long long minFrameTime = 1000 / m_targetFramerate;
	Timer timer;

	Graphics& m_gfx = *m_window->m_gfx;

	while (true) {

		if (const auto ecode = Window::ProcessMessages()) {
			return *ecode;
		}

		if (m_activeScene) {
			m_activeScene->Traverse();
		}

		m_scriptDispatcher.DispatchScripts();

		for (CallbackFunc func : m_preRenderCallbacks) {
			func(m_dt);
		}

		m_renderer->Render();

		for (CallbackFunc func : m_postRenderCallbacks) {
			func(m_dt);
		}

		m_gfx.SwapBuffers();		

		long long elapsed = timer.Peek();
		long long to_sleep = minFrameTime - elapsed;
		to_sleep = to_sleep < 0 ? 0 : to_sleep;
		std::this_thread::sleep_for(std::chrono::milliseconds(to_sleep));
		timer.Mark();

		m_dt = (float)(elapsed + to_sleep)*0.001f;
	}



	return 0;
}