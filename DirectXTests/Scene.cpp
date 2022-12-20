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
}

Node* Scene::AddNode(Entity* entity, const Transform& transform, Node* m_parent, bool serializable) {
	if (!m_parent) m_parent = &m_transformHierarchy;
	Node* node = new Node(serializable);
	if(entity) node->m_entities = { entity };
	node->m_parent = m_parent;
	node->m_localTransform = transform;
	m_parent->m_children.push_back(node);
	return node;
} 

Node* Scene::AddNode(std::vector<Entity*> m_entities, const Transform& transform, Node* m_parent, bool serializable) {
	if (!m_parent) m_parent = &m_transformHierarchy;
	Node* node = new Node(serializable);
	node->m_entities = m_entities;
	node->m_parent = m_parent;
	node->m_localTransform = transform;
	m_parent->m_children.push_back(node);
	return node;
}

void Scene::Traverse() {
	TraverseNode(&m_transformHierarchy, false);
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


