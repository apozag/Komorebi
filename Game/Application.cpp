
#define NOMINMAX
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <exception>
#include <Windows.h>
#include "Exception.h"
#include "Engine.h"
#include "Scene.h"
#include "Pass.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "Light.h"
#include "DepthStencilState.h"
#include "CubeTexture.h"
#include "Rasterizer.h"
#include "ModelLoader.h"
#include "TagManager.h"
#include "Rotate.h"
#include "ChangeColor.h"
#include "BindableSlotsInfo.h"

// Scripts
#include "CameraMovement.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow){
	try {

		Engine::Init("MyGame", 1024, 1024, 60);

		Scene* scene = new Scene();

		// RTs
		RenderTarget* drt = Engine::GetDefaultRendertarget();

		// Tags
		TagManager::GetInstance()->RegisterTag("UI");

		// Lights
		DirectionalLight* dirLight = new DirectionalLight(DirectX::SimpleMath::Vector3(1, 1, 1));
		Node* lightNode = scene->AddNode(dirLight, Transform(
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationRollPitchYawFromVector({ 3.14 * 0.25, 0, 0 }),
				DirectX::XMMatrixTranslationFromVector({ 0, 300, -300 })
			)
		), nullptr);

		// Cameras
		Camera* camera = new Camera(1.0472f, 1, 0.1, 500, drt, false);
		CameraMovement* cameraMovement = new CameraMovement();
		Node* cameraNode = scene->AddNode({ camera, cameraMovement }, Transform(
			DirectX::XMMatrixTranslationFromVector({ 0, 100, 0 })
		));
		camera->m_tagMask = ~TagManager::GetInstance()->TagToBitmask("UI");

		// Passes
		Pass* defaultPass = new Pass("shaders/SkinnedVertex.cso", "shaders/SkinnedPixel.cso", PASSLAYER_OPAQUE, true);
		Pass* shadowPass = new Pass("shaders/ShadowVertex.cso", "shaders/ShadowPixel.cso", PASSLAYER_OPAQUE);
		Pass* modelPass = new Pass("shaders/ShadowVertex.cso", "shaders/TexturedModelPixel.cso", PASSLAYER_OPAQUE);
		Pass* aabbPass = new Pass("shaders/cubeVertex.cso", "shaders/SolidPixel.cso", PASSLAYER_OPAQUE);
		Pass* skyboxPass = new Pass("shaders/skyboxVertex.cso", "shaders/skyboxPixel.cso", PASSLAYER_SKYBOX);

		DepthStencilState* dsState = new DepthStencilState(
			DepthStencilState::DepthStencilAccess::DEPTH_READ |
			DepthStencilState::DepthStencilAccess::DEPTH_WRITE
		); 

		defaultPass->AddBindable(dsState);

		shadowPass->AddBindable(dsState);

		aabbPass->AddBindable(new Rasterizer(true, true));
		aabbPass->AddBindable(dsState);

		skyboxPass->AddBindable(new DepthStencilState(DepthStencilState::DepthStencilAccess::DEPTH_READ));

		modelPass->AddBindable(dsState);


		// Materials
		Material* skinnedMat = new Material();
		skinnedMat->AddPass(defaultPass);

		Material* skyboxMat = new Material();
		skyboxMat->AddPass(skyboxPass);
		skyboxMat->AddBindable(new CubeTexture("assets/skybox", SRV_FREE_SLOT));

		Material* shadowMat = new Material();
		shadowMat->AddPass(shadowPass);

		Material* aabbMat = new Material();
		aabbMat->AddPass(aabbPass);

		//Models
		Node* modelWrapperNode = scene->AddNode(nullptr, Transform( 
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationY(3.14f),
				DirectX::XMMatrixTranslationFromVector({ 0, 0, -100 })
			))
		);

		//Model* model = ModelLoader::LoadModel("assets/huesitos.fbx", scene, modelWrapperNode);
		//Model* model = ModelLoader::LoadModel("assets/moneco.fbx", scene, nullptr);;
		Model* model1 = new Model("assets/Jump.fbx", scene, modelWrapperNode);;
		//Model* model = ModelLoader::LoadModel("assets/nanosuit/nanosuit.fbx", scene, modelWrapperNode);
		//Model* model = ModelLoader::LoadModel("assets/Female1.fbx", scene, modelWrapperNode);
		//Model* model = ModelLoader::LoadModel("assets/scan_model.fbx", scene, modelWrapperNode);
		Model* model = new Model ("assets/demon.fbx", scene, nullptr);

		model->AddPass(defaultPass);
		model1->AddPass(shadowPass);

		Mesh* floor = ModelLoader::GenerateQuad();
		floor->m_material = shadowMat;
		Node* floorNode = scene->AddNode({ floor  }, Transform(
			DirectX::XMMatrixScalingFromVector({ 1000, 1000, 1 }) * DirectX::XMMatrixRotationX(3.14f * 0.5f)
		));

		Mesh* skybox = ModelLoader::GenerateCube();
		skybox->m_material = skyboxMat;
		skybox->m_tagMask = TagManager::GetInstance()->TagToBitmask("Skybox");
		scene->AddNode(skybox, Transform());
		
		Drawable::BVHData bvhData = model->GetBVHData();
		Mesh* AABB = ModelLoader::GenerateAABB(bvhData.min, bvhData.max);
		AABB->m_material = aabbMat;
		scene->AddNode(AABB, Transform());
		
		//Mesh* fullScreenQuad = ModelLoader::GenerateQuad(scene, nullptr);
		//fullScreenQuad->AddPass(postProcessPass);

		Engine::m_activeScene = scene;

		using namespace rapidxml;
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* rootNode = doc.allocate_node(rapidxml::node_type::node_element, "Scene");
		doc.append_node(rootNode);
		Scene::Reflection.serialize(scene, rootNode, &doc);

		std::ofstream myfile;
		myfile.open("example.txt");
		myfile << doc;
		myfile.close();

		return Engine::Run();
	}
	catch (const Exception& e) {
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, e.what(), "Generic Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);

	}
	return -1;
}

