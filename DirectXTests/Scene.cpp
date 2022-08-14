#define NOMINMAX

#include "Scene.h"
#include "Renderer.h"
#include "Pass.h"
#include "DepthStencilState.h"
#include "CubeTexture.h"
#include "ConstantBuffer.h"
#include "RenderTarget.h"
#include "Rasterizer.h"
#include "BlendState.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Light.h"

void Scene::LoadScene( ) {
	/*
	// RTs
	RenderTarget* drt = gfx.GetDefaultRenderTarget();

	// Cameras
	Camera* camera = new Camera ( 1.0472f, 1, 0.1, 500, drt, false);
	m_mainCamera = AddNode(camera, Transform(
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationRollPitchYawFromVector({ 3.14 * 0.25, 0, 0 }),
			DirectX::XMMatrixTranslationFromVector({ 0, 30, -20 })
		)
	));

	// Lights
	DirectionalLight* dirLight = new DirectionalLight ( DirectX::SimpleMath::Vector3(1, 1, 1));
	Node* lightNode = AddNode(dirLight, Transform(
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationRollPitchYawFromVector({ 3.14 * 0.25, 0, 0 }),
			DirectX::XMMatrixTranslationFromVector({ 0, 30, 0 })
		)
	), m_mainCamera);

	// Passes
	Pass* defaultPass = new Pass ( "SkinnedVertex.cso", "SkinnedPixel.cso", PASSLAYER_OPAQUE, true);
	Pass* shadowPass = new Pass ( "ShadowVertex.cso", "ShadowPixel.cso", PASSLAYER_OPAQUE);
	Pass* aabbPass = new Pass ( "cubeVertex.cso", "SolidPixel.cso", PASSLAYER_OPAQUE);

	defaultPass->AddBindable(new DepthStencilState (
		DepthStencilState::DepthStencilAccess::DEPTH_READ | 
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));

	shadowPass->AddBindable(new DepthStencilState (
		DepthStencilState::DepthStencilAccess::DEPTH_READ |
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));

	aabbPass->AddBindable(new Rasterizer ( true, true));
	aabbPass->AddBindable(new DepthStencilState (
		DepthStencilState::DepthStencilAccess::DEPTH_READ |
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));
	
	Pass* skyboxPass = new Pass ( "skyboxVertex.cso", "skyboxPixel.cso", PASSLAYER_SKYBOX);
	skyboxPass->AddBindable(new DepthStencilState ( DepthStencilState::DepthStencilAccess::DEPTH_READ));
	skyboxPass->AddBindable(new CubeTexture ( "assets/skybox", 0));
	
	//Models
	Node* modelWrapperNode = AddNode(nullptr, Transform(DirectX::XMMatrixScaling(0.1, 0.1, 0.1)));
	//Model* model = ModelLoader::LoadModel ( "assets/rp_claudia_rigged_002_u3d.fbx", this, modelWrapperNode);
	Model* model = ModelLoader::LoadModel ( "assets/huesitos.fbx", this, modelWrapperNode);// , modelWrapperNode);
	//Model* model = ModelLoader::LoadModel ( "assets/animation_with_skeleton.fbx", this, modelWrapperNode);
	//Model* model = ModelLoader::LoadModel ( "assets/nanosuit/nanosuit.obj", this, modelWrapperNode);
	//Model model ( "assets/box1.fbx", this, modelWrapperNode);
	//Model model1 ( "assets/C96/C96.fbx", this, modelWrapperNode);

	model->AddPass(defaultPass);

	Mesh* floor = ModelLoader::GenerateQuad ( this, AddNode(nullptr, Transform(
		DirectX::XMMatrixScalingFromVector({ 100, 100, 1 }) * DirectX::XMMatrixRotationX(3.14f * 0.5f)
	)));
	floor->AddPass(shadowPass);

	Mesh* skybox = ModelLoader::GenerateCube ( this, nullptr);
	skybox->AddPass(skyboxPass);

	
	Drawable::BVHData bvhData = model->GetBVHData();
	Mesh* AABB = ModelLoader::GenerateAABB ( bvhData.min, bvhData.max, this , modelWrapperNode);
	AABB->AddPass(aabbPass);		
	*/
}

Node* Scene::AddNode(Entity* entity, const Transform& transform, Node* parent) {
	if (!parent) parent = &m_transformHierarchy;
	Node* node = new Node();
	node->entities = { entity };
	node->parent = parent;
	node->localTransform = transform;
	parent->children.push_back(node);
	return node;
} 

Node* Scene::AddNode(std::vector<Entity*> entities, const Transform& transform, Node* parent) {
	if (!parent) parent = &m_transformHierarchy;
	Node* node = new Node();
	node->entities = entities;
	node->parent = parent;
	node->localTransform = transform;
	parent->children.push_back(node);
	return node;
}

void Scene::Traverse() {
	TraverseNode(&m_transformHierarchy, false);
}

void Scene::TraverseNode(Node* node, bool dirty) {

	dirty |= node->localTransform.m_dirty;

	if (dirty) {
		if (!node->parent) {
			node->globalTransform = node->localTransform;
		}
		else {
			node->globalTransform = node->localTransform * node->parent->globalTransform;
		}
		node->localTransform.m_dirty = false;
	}

	for (Entity* entity : node->entities) {
		if (entity) {
			node->globalTransform.update();
			entity->Insert(node, node->globalTransform);
		}
	}

	for (Node* child : node->children) {
		TraverseNode(child, dirty);
	}
}


