
#define NOMINMAX
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
	int		  nCmdShow)
{
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
				DirectX::XMMatrixTranslationFromVector({ 0, 100, -100 })
			)
		), nullptr);

		// Cameras
		Camera* camera = new Camera(1.0472f, 1, 0.1, 500, drt, false);
		//camera->m_priority = 0;
		CameraMovement* cameraMovement = new CameraMovement();
		Node* cameraNode = scene->AddNode({ camera, cameraMovement }, Transform(
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationRollPitchYawFromVector({ 0, 3.14, 0 }),
				DirectX::XMMatrixTranslationFromVector({ 0, 20, 100 })
			)
		));
		camera->m_tagMask = ~TagManager::GetInstance()->TagToBitmask("UI");

		// Passes
		Pass* defaultPass = new Pass("SkinnedVertex.cso", "SkinnedPixel.cso", PASSLAYER_OPAQUE, true);
		Pass* shadowPass = new Pass("ShadowVertex.cso", "ShadowPixel.cso", PASSLAYER_OPAQUE);
		Pass* modelPass = new Pass("ShadowVertex.cso", "TexturedModelPixel.cso", PASSLAYER_OPAQUE);
		Pass* aabbPass = new Pass("cubeVertex.cso", "SolidPixel.cso", PASSLAYER_OPAQUE);
		Pass* skyboxPass = new Pass("skyboxVertex.cso", "skyboxPixel.cso", PASSLAYER_SKYBOX);

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
		skyboxMat->AddBindable(new CubeTexture("assets/skybox", TEX2D_FREE_SLOT));

		Material* shadowMat = new Material();
		shadowMat->AddPass(shadowPass);

		Material* aabbMat = new Material();
		aabbMat->AddPass(aabbPass);

		//Models
		Node* modelWrapperNode = scene->AddNode(nullptr, Transform(DirectX::XMMatrixScaling(0.1, 0.1, 0.1)));

		//Model* model = ModelLoader::LoadModel("assets/huesitos.fbx", scene, modelWrapperNode);
		//Model* model = ModelLoader::LoadModel("assets/ybot.fbx", scene, nullptr);;
		Model* model = ModelLoader::LoadModel("assets/nanosuit/nanosuit.fbx", scene, modelWrapperNode);

		model->AddPass(modelPass);

		Mesh* floor = ModelLoader::GenerateQuad();
		floor->m_material = shadowMat;
		Node* floorNode = scene->AddNode({ floor  }, Transform(
			DirectX::XMMatrixScalingFromVector({ 100, 100, 1 }) * DirectX::XMMatrixRotationX(3.14f * 0.5f)
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
		return Engine::Start();
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

