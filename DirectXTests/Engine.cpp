#include <thread>

#include "Engine.h"
#include "Timer.h"
#include "Renderer.h"
#include "Scene.h"
#include "Window.h"
#include "Script.h"
#include "ScriptDispatcher.h"


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

int Engine::Start() 	
{	
	long long minFrameTime = 1000 / m_targetFramerate;
	Timer timer;

	Graphics& gfx = *m_window->gfx;

	//m_activeScene->LoadScene ;

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

		///////// PROVISIONAL///////////
		/*
		Transform& cameraTransform = m_activeScene->GetMainCameraNode()->localTransform;

		float rotateSpeed = 0.3f;
		static float lastX = m_window->mouse.GetX();
		static float lastY = m_window->mouse.GetX();
		if (m_window->mouse.IsLeftPressed()) {
			float x = m_window->mouse.GetX();
			float y = m_window->mouse.GetY();
			float rotateR = (y - lastY) * rotateSpeed * m_dt;
			float rotateU = (x - lastX) * rotateSpeed * m_dt;

			cameraTransform.RotateEulerLocal(DirectX::SimpleMath::Vector3(rotateR, rotateU, 0));

			lastX = x;
			lastY = y;
		}
		lastX = m_window->mouse.GetX();
		lastY = m_window->mouse.GetY();

		float moveSpeed = 5.0f;

		if (m_window->keyboard.KeyIsPressed('W')) {
			cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 0, 1) * moveSpeed * m_dt);
		}
		if (m_window->keyboard.KeyIsPressed('S')) {
			cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 0, 1) * -moveSpeed * m_dt);
		}
		if (m_window->keyboard.KeyIsPressed('D')) {
			cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(1, 0, 0) * moveSpeed * m_dt);
		}
		if (m_window->keyboard.KeyIsPressed('A')) {
			cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(1, 0, 0) * -moveSpeed * m_dt);
		}
		if (m_window->keyboard.KeyIsPressed('E')) {
			cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 1, 0) * moveSpeed * m_dt);
		}
		if (m_window->keyboard.KeyIsPressed('Q')) {
			cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 1, 0) * -moveSpeed * m_dt);
		}
		*/
		////////////////////////////////
	}

	return 0;
}