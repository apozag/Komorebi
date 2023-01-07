#include <iostream>
#include <fstream>

#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml.hpp"

#include "SceneLoader.h"
#include "ReflectionMacros.h"
#include "Scene.h"

using namespace reflection;
using namespace rapidxml;

constexpr const char* nodeTypeName = "Node";
constexpr const char* transformTypeName = "Transform";
constexpr const char* entitiesNodeName = "Entities";
constexpr const char* childrenNodeName = "Children";


Scene* SceneLoader::LoadScene(const char* filename) {
  rapidxml::file<> xmlFile(filename);
  xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  const xml_node<>* nodeElem = doc.first_node(nodeTypeName);

  Scene* scene = new Scene();
  Node::GetReflection().deserialize(scene->GetRootNode(), nodeElem);
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