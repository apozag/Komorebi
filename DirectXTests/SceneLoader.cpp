#include <iostream>
#include <fstream>
#include "rapidxml.hpp"

#include "SceneLoader.h"
#include "Reflection.h"
#include "Scene.h"

using namespace reflect;
using namespace rapidxml;

constexpr const char* nodeTypeName = "Node";
constexpr const char* transformTypeName = "Transform";
constexpr const char* entitiesNodeName = "Entities";
constexpr const char* childrenNodeName = "Children";


void SceneLoader::RegisterTypeDesc(TypeDescriptor* typeDesc) {
  GetTypeDict().insert({ typeDesc->name, typeDesc });
}

 void SceneLoader::TrackString(std::string* string) {
  GetStrVector().push_back(string);
}

TypeDescriptor* SceneLoader::GetTypeDesc(std::string name) {
  return GetTypeDict()[name];
}

Scene* SceneLoader::LoadScene(const char* filename) {
  rapidxml::file<> xmlFile(filename);
  xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  xml_node<>* nodeElem = doc.first_node(nodeTypeName);

  Scene* scene = new Scene();

  while (nodeElem) {
    ParseNode(nodeElem, scene->GetRootNode());
    nodeElem = nodeElem->next_sibling(nodeTypeName);
  }

  return scene;
}

void SceneLoader::SaveScene(Scene* scene, const char* filename) {
  xml_document<> doc;
  SerializeNode(&doc, &doc, scene->GetRootNode());

  std::ofstream myfile;
  myfile.open(filename);
  myfile << doc;
  myfile.close();

  for (std::string* strPtr : GetStrVector()) {
    delete strPtr;
  }
  GetStrVector().clear();
}

void SceneLoader::ParseNode(xml_node<>* elemNode, Node* parent) {
  Node* node = new Node(true);
  node->m_parent = parent;
  parent->m_children.push_back(node);
  
  xml_node<>* transformElem = elemNode->first_node(transformTypeName);
  if (transformElem) {
    GetTypeDesc(transformTypeName)->deserialize(&node->m_localTransform, transformElem);
  }

  xml_node<>* entitiesElem = elemNode->first_node(entitiesNodeName);
  if (entitiesElem) {
    xml_node<>* entityElem = entitiesElem->first_node();
    while (entityElem) {
      TypeDescriptor* pTypeDesc = GetTypeDesc(entityElem->name());
      void* obj = malloc(pTypeDesc->size);
      pTypeDesc->deserialize(obj, entityElem);
      node->m_entities.push_back(static_cast<Entity*>(obj));
      entityElem = entityElem->next_sibling();
    }
  }

  xml_node<>* childrenElem = elemNode->first_node(childrenNodeName);
  if (childrenElem) {
    xml_node<>* childElem = childrenElem->first_node(nodeTypeName);
    while (childElem) {
      ParseNode(childElem, node);
      childElem = childElem->next_sibling();
    }
  }

}

void SceneLoader::SerializeNode(rapidxml::xml_node<>* elemNode, rapidxml::xml_document<>* doc, Node* node) {

  xml_node<>* transform_elem = doc->allocate_node(node_type::node_element, transformTypeName);
  elemNode->append_node(transform_elem);
  node->m_localTransform.GetReflection().serialize(&node->m_localTransform, transform_elem, doc);

  xml_node<>* entities_elem = doc->allocate_node(node_type::node_element, entitiesNodeName);
  elemNode->append_node(entities_elem);
  for (Entity* entity : node->m_entities) {
    const TypeDescriptor& typeDesc = entity->GetReflectionDerived();
    xml_node<>* entity_elem = doc->allocate_node(node_type::node_element, typeDesc.name);
    entities_elem->append_node(entity_elem);
    typeDesc.serialize(entity, entity_elem, doc);
  }

  xml_node<>* children_elem = doc->allocate_node(node_type::node_element, childrenNodeName);
  elemNode->append_node(children_elem);
  for (Node* child : node->m_children) {
    if (!child->IsSerializable()) {
      continue;
    }
    xml_node<>* child_elem = doc->allocate_node(node_type::node_element, nodeTypeName);
    children_elem->append_node(child_elem);
    SerializeNode(child_elem, doc, child);
  }
}