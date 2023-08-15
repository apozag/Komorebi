#define NOMINMAX

#include "Scene/Scene.h"

#include "Core/Memory/Factory.h"
#include "Core/Reflection/ReflectionImplMacros.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Scene/Node.h"
#include "Scene/AuxNode.h"
#include "Graphics/Renderer.h"
#include "Graphics/Pass.h"
#include "Graphics/Bindables/State/DepthStencilState.h"
#include "Graphics/Bindables/Resource/CubeTexture.h"
#include "Graphics/Bindables/Resource/ConstantBuffer.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Graphics/Bindables/State/RasterizerState.h"
#include "Graphics/Bindables/State/BlendState.h"
#include "Entities/Model.h"
#include "Scene/ModelLoader.h"
#include "Entities/Camera.h"
#include "Entities/Light.h"

std::vector<Scene::EntityAddFunc> Scene::s_entityTypes; 

void Scene::LoadScene( ) {
}

Node* Scene::AddNode(Entity* entity, const Transform& transform, Node* m_parent, bool auxNode) {
	if (!m_parent) m_parent = m_transformHierarchy;
	Node* node = auxNode? memory::Factory::Create<AuxNode>() : memory::Factory::Create<Node>();
	if (entity) {
		node->m_entities = { entity };		
		entity->m_node = node;
	}
	node->m_parent = m_parent;
	node->m_localTransform = transform;
	m_parent->m_children.push_back(node);
	return node;
} 

Node* Scene::AddNode(std::vector<Entity*> entities, const Transform& transform, Node* m_parent, bool auxNode) {
	if (!m_parent) m_parent = m_transformHierarchy;
	Node* node = memory::Factory::Create<Node>();
	for (Entity* pEntity : entities) {
		node->m_entities.push_back(pEntity);
		pEntity->m_node = node;
	}
	node->m_parent = m_parent;
	node->m_localTransform = transform;
	m_parent->m_children.push_back(node);
	return node;
}

Node* Scene::AddPrefabNode(const char* filename, const Transform& transform, Node* m_parent) {

	Node* node = PrefabManager::GetInstance()->LoadPrefab<Node>(filename, false);
	if (node != nullptr) {
		if (!m_parent) m_parent = m_transformHierarchy;
		node->m_parent = m_parent;
		node->m_localTransform = transform;
		m_parent->m_children.push_back(node);
		// Entities of "node" will not have found its node pointer, becouse "node" was deserialized as struct and not owned_ptr
		// Therefore, we link them here manually (kind of shitty)
		for (Entity* entity : node->m_entities) {
			entity->m_node = node;
		}
		reflection::SetupTypeVisitor setupVisitor((void*)node);
		reflection::TypeResolver<Node>::get()->Accept(&setupVisitor);
	}
	return node;
}

Entity* Scene::AddEntity(Node* node, const char* typeName) {
	for (Scene::EntityAddFunc& enttFunc : s_entityTypes) {
		if (strcmp(enttFunc.m_name.c_str(),  typeName) == 0) {			
			return enttFunc.addEntity(node);
		}
	}
	// TODO: [ERROR] No entity "typeName" found

	return nullptr;
}

std::vector<std::string> Scene::GetEntityNames() {
	std::vector<std::string> names;
	for (EntityAddFunc& enttFunc : s_entityTypes) {
		names.push_back(enttFunc.m_name);
	}
	return names;
}

void Scene::Traverse() {
	TraverseNode(m_transformHierarchy, false);
}

void Scene::TraverseNode(Node* node, bool dirty) {

	if (!node || !node->m_enabled) {
		return;
	}

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
