#include <thread>

#include "Core/Engine.h"
#include "Core/Memory/Factory.h"
#include "Core/Time/Timer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Graphics.h"
#include "Scene/Scene.h"
#include "Core/Window.h"
#include "Entities/Script.h"
#include "Entities/Camera.h"
#include "Core/ScriptDispatcher.h"
#include "Core/Window.h"

gfx::Renderer* Engine::m_renderer;
Window* Engine::m_window;
Scene* Engine::m_activeScene;
Camera* Engine::m_mainCamera;
ScriptDispatcher Engine::m_scriptDispatcher;
std::vector<Engine::CallbackFunc> Engine::m_preRenderCallbacks;
std::vector<Engine::CallbackFunc> Engine::m_postRenderCallbacks;
float Engine::m_dt;
float Engine::m_targetFramerate;

gfx::RenderTarget* Engine::GetDefaultRendertarget() {
	return m_window->m_gfx->GetDefaultRenderTarget();
}

void Engine::Init(const char* windowTitle, int windowWidth, int windowHeight, float targetFramerate) {
	memory::Factory::Init(1024*32, 1024 * 1024);
	memory::Factory::SetGlobalMode(true);
	m_window = memory::Factory::Create<Window>(windowWidth, windowHeight, windowTitle);
	m_window->Init();
	m_renderer = memory::Factory::Create<gfx::Renderer>();
	m_targetFramerate = targetFramerate;
	m_renderer->Init();
	memory::Factory::SetGlobalMode(false);
}

int Engine::Run() 	
{	
	long long minFrameTime = 1000 / m_targetFramerate;
	Timer timer;

	gfx::Graphics& m_gfx = *m_window->m_gfx;

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

		m_renderer->Clear();

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

Camera* RecursiveFindMainCamera(Node* node) {
	for (Entity* entity : node->m_entities) {
		Camera* camera = dynamic_cast<Camera*>(entity);
		if (camera && camera->m_tag == MAIN_CAMERA_TAG) {
			return camera;
		}
	}

	for (Node* child : node->m_children) {
		Camera* mainCamera = RecursiveFindMainCamera(child);
		if (mainCamera != nullptr) {
			return mainCamera;
		}
	}

	return nullptr;
}

void Engine::SetActiveScene(Scene* scene) {
	m_activeScene = scene;
	m_mainCamera = m_activeScene ? RecursiveFindMainCamera(m_activeScene->m_transformHierarchy) : nullptr;
}
