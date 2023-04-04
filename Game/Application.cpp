
#define NOMINMAX
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <exception>
#include <Windows.h>

#include "Core/Exceptions/Exception.h"
#include "Core/Engine.h"
#include "Core/Memory/Factory.h"
#include "Scene/Scene.h"
#include "Graphics/Pass.h"
#include "Graphics/Material.h"
#include "Entities/Model.h"
#include "Entities/Mesh.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Entities/Light.h"
#include "Graphics/Bindables/State/DepthStencilState.h"
#include "Graphics/Bindables/Resource/CubeTexture.h"
#include "Graphics/Bindables/State/RasterizerState.h"
#include "Scene/ModelLoader.h"
#include "Core/Util/TagManager.h"
#include "Rotate.h"
#include "ChangeColor.h"
#include "Graphics/BindableSlotsInfo.h"
#include "Scene/SceneLoader.h"

#include "Core/Time/Timer.h"

// Scripts
#include "CameraMovement.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow){
	try {

		Engine::Init("MyGame", 1024, 1024, 60);
		
		return 0;
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

