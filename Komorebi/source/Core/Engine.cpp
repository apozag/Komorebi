#include <thread>

#include "Core/Engine.h"
#include "Core/Time/Timer.h"
#include "Graphics/Renderer.h"
#include "Scene/Scene.h"
#include "Core/Window.h"
#include "Entities/Script.h"
#include "Core/ScriptDispatcher.h"


Renderer* Engine::m_renderer;
Window* Engine::m_window;
Scene* Engine::m_activeScene;
ScriptDispatcher Engine::m_scriptDispatcher;
float Engine::m_dt;
float Engine::m_targetFramerate;

void Engine::Init(const char* windowTitle, int windowWidth, int windowHeight, float targetFramerate) {
	m_window = new Window(windowWidth, windowHeight, windowTitle);
	m_window->gfx->Init();
	m_renderer = new Renderer();
	m_targetFramerate = targetFramerate;
}

int Engine::Run() 	
{	
	long long minFrameTime = 1000 / m_targetFramerate;
	Timer timer;

	Graphics& gfx = *m_window->gfx;

	while (true) {

		if (const auto ecode = Window::ProcessMessages()) {
			return *ecode;
		}

		m_activeScene->Traverse ();

		m_scriptDispatcher.DispatchScripts();

		m_renderer->Render();
		gfx.SwapBuffers();		

		long long elapsed = timer.Mark();
		long long to_sleep = minFrameTime - elapsed;
		to_sleep = to_sleep < 0 ? 0 : to_sleep;
		std::this_thread::sleep_for(std::chrono::milliseconds(to_sleep));

		m_dt = (float)(elapsed + to_sleep)*0.001f;
	}



	return 0;
}