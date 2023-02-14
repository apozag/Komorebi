#include <iostream>
#include <fstream>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Engine.h"
#include "Scene/SceneLoader.h"
#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/TypeVisitor.h"
#include "Scene/Scene.h"

using namespace reflection;
using namespace rapidxml;

constexpr const char* nodeTypeName = "Scene";

Scene* SceneLoader::LoadScene(const char* filename) {
  rapidxml::file<> xmlFile(filename);
  xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  const xml_node<>* nodeElem = doc.first_node(nodeTypeName);

  Scene* scene = new Scene();
  Engine::m_activeScene = scene;
  //Scene::GetReflection().deserialize(scene, nodeElem);

  ReflectionHelper::ResolvePendingPointers();

  return scene;
}

void SceneLoader::SaveScene(Scene* scene, const char* filename) {
  xml_document<> doc;
  SerializationTypeVisitor visitor(scene, &doc);
  const TypeDescriptor_Struct& type = scene->GetReflection();
  type.Accept(&visitor);

  ReflectionHelper::ResolvePendingPointers();
  std::ofstream myfile;
  myfile.open(filename);
  myfile << doc;
  myfile.close();

  ReflectionHelper::ClearTrackedStrings();
}