#pragma once

#include <vector>
#include <string>

#include "Scene/Node.h"
#include "Core/Memory/Factory.h"
#include "Core/Reflection/ReflectionMacros.h"

class Entity;

class Scene : public GameObject{
public:
	Scene() = default;
	~Scene() = default;
	void LoadScene( );

	void Traverse();

	void Setup() {}

	Node* AddNode(Entity* entity, const Transform& transform, Node* m_parent = nullptr, bool auxNode = false);
	Node* AddNode(std::vector<Entity*> m_entities, const Transform& transform, Node* m_parent = nullptr, bool auxNode = false);
	Node* AddPrefabNode(const char* filename, const Transform& transform, Node* m_parent = nullptr);

	template<class T, typename ...Args>
	static T* AddEntity(Node* node, Args&&... args) {
		if (!node) return nullptr;
		T* entity = memory::Factory::Create<T>(std::forward(args)...);
		node->m_entities.push_back(entity);
		entity->m_node = node;
		return entity;
	}

	static Entity* AddEntity(Node* node, const char* typeName);

	Node* GetRootNode() { return m_transformHierarchy; }

	void Serialize(const char* filename);

	template<class T>
	static int RegisterEntity() {
		s_entityTypes.push_back({
			std::string(T::GetReflection().name),
			[](Node* node) -> Entity* {
				return Scene::AddEntity<T>(node);
			}
		});
		return 0;
	}

	static std::vector<std::string> GetEntityNames();

	REFLECT_BASE()

	OWNED_PTR(Node) m_transformHierarchy;

private:

	struct EntityAddFunc {
		std::string m_name;
		Entity* (*addEntity)(Node*);
	};

	static std::vector<EntityAddFunc> s_entityTypes;

	void TraverseNode(Node* node, bool dirty);

};	

#define REGISTER_ENTITY(TYPE) const int __reg_  ##TYPE = Scene::RegisterEntity<TYPE>();

