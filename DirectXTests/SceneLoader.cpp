#include <iostream>
#include <fstream>

#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml.hpp"

#include "Engine.h"
#include "SceneLoader.h"
#include "ReflectionMacros.h"
#include "Scene.h"

using namespace reflection;
using namespace rapidxml;

constexpr const char* nodeTypeName = "Node";

void LinkNodeChildrenRecursive(Node* node) {
  for (Node* child : node->m_children) {
    child->m_parent = node;
    LinkNodeChildrenRecursive(child);
  }
}

Scene* SceneLoader::LoadScene(const char* filename) {
  rapidxml::file<> xmlFile(filename);
  xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  const xml_node<>* nodeElem = doc.first_node(nodeTypeName);

  Scene* scene = new Scene();
  Engine::m_activeScene = scene;
  Node::GetReflection().deserialize(scene->GetRootNode(), nodeElem);

  LinkNodeChildrenRecursive(scene->GetRootNode());

  return scene;
}

void SceneLoader::SaveScene(Scene* scene, const char* filename) {
  xml_document<> doc;
  Node::GetReflection().serialize(scene->GetRootNode(), nodeTypeName, &doc, &doc);

  std::ofstream myfile;
  myfile.open(filename);
  myfile << doc;
  myfile.close();

  ReflectionHelper::ClearTrackedStrings();
}