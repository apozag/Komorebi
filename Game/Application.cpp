
#define NOMINMAX
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <exception>
#include <Windows.h>

#include "Core/Exceptions/Exception.h"
#include "Core/Engine.h"
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

void SaveScene() {
	Scene* scene = new Scene();
	scene->m_transformHierarchy = new Node();

	Engine::m_activeScene = scene;

	// RTs
	RenderTarget* drt = Engine::GetDefaultRendertarget();

	// Tags
	TagManager::GetInstance()->RegisterTag("UI");	

	// Cameras
	Camera* camera = new Camera(1.0472f, 1, 0.1, 500, drt, false);
	CameraMovement* cameraMovement = new CameraMovement();
	Node* cameraNode = scene->AddNode({ camera, cameraMovement }, Transform(
		DirectX::XMMatrixTranslationFromVector({ 0, 100, 0 })
	));

	Pass* skyboxPass = new Pass("assets/shaders/skyboxVertex.cso", "assets/shaders/skyboxPixel.cso", PASSLAYER_SKYBOX);
	skyboxPass->AddBindable(new DepthStencilState(DepthStencilState::DepthStencilAccess::DEPTH_READ));

	Pass* defaultPass = new Pass("assets/shaders/SkinnedVertex.cso", "assets/shaders/SkinnedPixel.cso", PASSLAYER_OPAQUE, true);
	defaultPass->AddBindable(new DepthStencilState(
		DepthStencilState::DepthStencilAccess::DEPTH_READ |
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));

	Model* skybox = new Model("cube");
	skybox->AddPass(skyboxPass);
	skybox->AddBindable(new CubeTexture("assets/images/skybox", SRV_FREE_SLOT));
	scene->AddNode(skybox, Transform());

	Model* model = new Model("assets/models/demon.fbx");
	model->AddPass(defaultPass);
	scene->AddNode(model, Transform());

	DirectionalLight* dirLight = new DirectionalLight(DirectX::SimpleMath::Vector3(1, 1, 1));
	scene->AddNode(dirLight, Transform(
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationRollPitchYawFromVector({ 3.14 * 0.25, 0, 0 }),
			DirectX::XMMatrixTranslationFromVector({ 0, 300, -300 })
		)
	), nullptr);
	
	SceneLoader::SaveScene(scene, "TESTTESTTEST.xml");
}

void LoadScene() {
	Engine::m_activeScene = SceneLoader::LoadScene("assets/scenes/testScene.xml");
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow){
	try {

		Engine::Init("MyGame", 1024, 1024, 60);

		//SaveScene();
		LoadScene();

		return Engine::Run();
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

