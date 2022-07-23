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

void Scene::LoadScene(Graphics& gfx) {

	// RTs
	RenderTarget* drt = gfx.GetDefaultRenderTarget();
	RenderTarget* rt0 = new RenderTarget(gfx, drt->GetWidth(), drt->GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

	// Lights
	DirectionalLight* dirLight = new DirectionalLight(gfx, DirectX::SimpleMath::Vector3(1, 1, 1));
	AddNode(dirLight, Transform(DirectX::XMMatrixLookAtLH({ 0,0,0 }, { -1,0,0 }, { 0,0,1 })));

	// Cameras
	Camera* camera = new Camera(gfx, 1.0472f, 1, 0.1, 1000, drt);
	m_mainCamera = AddNode(camera, Transform(DirectX::XMMatrixTranslation(0.0f, 5.0f, 20.0f)));
	Camera* camera1 = new Camera(gfx, 1.0472f, 1, 0.1, 100, rt0);
	AddNode(camera1, Transform(DirectX::XMMatrixTranslation(0.0f, 5.0f, -20.0f)));

	// Passes
	Pass* defaultPass = new Pass(gfx, "SkinnedVertex.cso", "DiffusePixel.cso", PASSLAYER_OPAQUE, true);
	Pass* solidPass = new Pass(gfx, "vertex.cso", "SolidPixel.cso", PASSLAYER_OPAQUE);
	Pass* screenPass = new Pass(gfx, "vertex.cso", "texturePixel.cso", PASSLAYER_OPAQUE);
	Pass* aabbPass = new Pass(gfx, "cubeVertex.cso", "SolidPixel.cso", PASSLAYER_OPAQUE);

	defaultPass->AddBindable(new DepthStencilState(gfx,
		DepthStencilState::DepthStencilAccess::DEPTH_READ | 
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));

	solidPass->AddBindable(new DepthStencilState(gfx,
		DepthStencilState::DepthStencilAccess::DEPTH_READ |
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));
	solidPass->AddBindable(new BlendState(gfx, true));

	screenPass->AddBindable(dirLight->GetShadowMap());
	screenPass->AddBindable(new Rasterizer(gfx, true));
	screenPass->AddBindable(new DepthStencilState(gfx,
		DepthStencilState::DepthStencilAccess::DEPTH_READ |
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));

	aabbPass->AddBindable(new Rasterizer(gfx, true, true));
	aabbPass->AddBindable(new DepthStencilState(gfx,
		DepthStencilState::DepthStencilAccess::DEPTH_READ |
		DepthStencilState::DepthStencilAccess::DEPTH_WRITE
	));
	
	Pass* skyboxPass = new Pass(gfx, "skyboxVertex.cso", "skyboxPixel.cso", PASSLAYER_SKYBOX);
	skyboxPass->AddBindable(new DepthStencilState(gfx, DepthStencilState::DepthStencilAccess::DEPTH_READ));
	skyboxPass->AddBindable(new CubeTexture(gfx, "assets/skybox", 0));
	
	//Models
	Node* modelWrapperNode = AddNode(nullptr, Transform(DirectX::XMMatrixScaling(0.1,0.1,0.1)));
	//Model model(gfx, "assets/huesitos.fbx", this, modelWrapperNode);
	Model* model = ModelLoader::LoadModel(gfx, "assets/rp_claudia_rigged_002_u3d.fbx", this, modelWrapperNode);
	//Model model(gfx, "assets/box1.fbx", this, modelWrapperNode);
	//Model model(gfx, "assets/animation_with_skeleton.fbx", this, modelWrapperNode);
	//Model model(gfx, "assets/nanosuit/nanosuit.obj", this, nullptr);
	//Model model1(gfx, "assets/C96/C96.fbx", this, modelWrapperNode);

	model->AddPass(defaultPass);

	Mesh* quad = ModelLoader::GenerateQuad(gfx, this, nullptr);
	quad->AddPass(screenPass);

	Mesh* floor = ModelLoader::GenerateQuad(gfx, this, AddNode(nullptr, Transform(
		DirectX::XMMatrixScalingFromVector({ 100, 100, 1 }) * DirectX::XMMatrixRotationX(3.14f * 0.5f)
	)));
	floor->AddPass(solidPass);

	Mesh* skybox = ModelLoader::GenerateCube(gfx, this, nullptr);
	skybox->AddPass(skyboxPass);

	Drawable::BVHData bvhData = model->GetBVHData();
	Mesh* AABB = ModelLoader::GenerateAABB(gfx, bvhData.min, bvhData.max, this, modelWrapperNode);
	AABB->AddPass(aabbPass);		
}

Node* Scene::AddNode(Entity* entity, const Transform& transform, Node* parent) {
	if (!parent) parent = &m_transformHierarchy;
	Node* node = new Node();
	node->entity = entity;
	node->parent = parent;
	node->localTransform = transform;
	parent->children.push_back(node);
	return node;
} 

void Scene::RenderTraverse(Graphics& gfx, Renderer& renderer) {
	RenderTraverseNode(&m_transformHierarchy, gfx, renderer, false);
}

void Scene::RenderTraverseNode(Node* node, Graphics& gfx, Renderer& renderer, bool dirty) {

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

	Entity* entity = node->entity;
	if (entity) {
		node->globalTransform.update();
		entity->Insert(renderer, node->globalTransform);
	}

	for (Node* child : node->children) {
		RenderTraverseNode(child, gfx, renderer, dirty);
	}

}


