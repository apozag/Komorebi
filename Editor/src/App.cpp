
#define NOMINMAX
#include <iostream>

#include <Windows.h>

#include "imgui/imgui.h"

#include "Core/Exceptions/Exception.h"
#include "Core/Engine.h"
#include "Core/Window.h"
#include "Scene/SceneLoader.h"
#include "GUI/GUI.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow) {
	try {

		Engine::Init("MyGame", 1024, 1024, 60);

		GUIAttachment attachment;

		Engine::GetWindow()->Attach(&attachment);

		Engine::m_activeScene = SceneLoader::LoadScene("assets/scenes/testScene.xml");

		return Engine::Run();
	}
	catch (const Exception& e) {
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Generic Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);

	}
	return -1;
}

