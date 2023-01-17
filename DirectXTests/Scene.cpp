#define NOMINMAX

#include "Scene.h"
#include "Renderer.h"
#include "Pass.h"
#include "DepthStencilState.h"
#include "CubeTexture.h"
#include "ConstantBuffer.h"
#include "RenderTarget.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Light.h"

void Scene::LoadScene( ) {
}

Node* Scene::AddNode(Entity* entity, const Transform& transform, Node* m_parent) {
	if (!m_parent) m_parent = m_transformHierarchy;
	Node* node = new Node();
	if(entity) node->m_entities = { entity };
	node->m_parent = m_parent;
	node->m_localTransform = transform;
	m_parent->m_children.push_back(node);
	return node;
} 

Node* Scene::AddNode(std::vector<Entity*> entities, const Transform& transform, Node* m_parent) {
	if (!m_parent) m_parent = m_transformHierarchy;
	Node* node = new Node();
	for (Entity* pEntity : entities) {
		node->m_entities.push_back(pEntity);
	}
	node->m_parent = m_parent;
	node->m_localTransform = transform;
	m_parent->m_children.push_back(node);
	return node;
}

void Scene::Traverse() {
	TraverseNode(m_transformHierarchy, false);
}

void Scene::TraverseNode(Node* node, bool dirty) {

	dirty |= node->m_localTransform.m_dirty;

	if (dirty) {
		if (!node->m_parent) {
			node->m_globalTransform = node->m_localTransform;
		}
		else {
			node->m_globalTransform = node->m_localTransform * node->m_parent->m_globalTransform;
		}
		node->m_localTransform.m_dirty = false;
	}

	for (Entity* entity : node->m_entities) {
		if (entity) {
			node->m_globalTransform.update();
			entity->Insert(node, node->m_globalTransform);
		}
	}

	for (Node* child : node->m_children) {
		TraverseNode(child, dirty);
	}
}

void Scene::Serialize(const char* filename) {
	
}

REFLECT_STRUCT_BASE_BEGIN(Scene)
REFLECT_STRUCT_MEMBER(m_transformHierarchy)
REFLECT_STRUCT_END(Scene)
