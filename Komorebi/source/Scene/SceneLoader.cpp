#include <iostream>
#include <fstream>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Engine.h"
#include "Core/Memory/Factory.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Scene/SceneLoader.h"
#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionSettings.h"
#include "Core/Reflection/SerializationTypeVisitor.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/UnloadTypeVisitor.h"
#include "Scene/Scene.h"
#include "Core/PrefabManager.h"

using namespace reflection;
using namespace rapidxml;

constexpr const char* nodeTypeName = "Scene";

std::string SceneLoader::m_lastLoadedFilename;

Scene* SceneLoader::LoadScene(const char* filename) {
  
  memory::Factory::PushGlobalMode(false);

  rapidxml::file<> xmlFile(filename);
  xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  const xml_node<>* nodeElem = doc.first_node(nodeTypeName);

  Scene* scene = memory::Factory::Create<Scene>();

  DeserializationTypeVisitor deserializationVisitor(scene, nodeElem);
  scene->GetReflection().Accept(&deserializationVisitor);

  Engine::SetActiveScene(scene);

  ReflectionHelper::ResolvePendingPointers();

  SetupTypeVisitor setupVisitor(scene);
  scene->GetReflection().Accept(&setupVisitor);

  ReflectionHelper::ClearAll();

  m_lastLoadedFilename = filename;

  memory::Factory::PopGlobalMode();

  return scene;
}

void SceneLoader::UnloadScene() {
  if (Engine::GetActiveScene()) {
    reflection::UnloadTypeVisitor visitor(Engine::GetActiveScene());
    reflection::__internal::forceVisitIgnored = true;
    Engine::GetActiveScene()->GetReflection().Accept(&visitor);
    reflection::__internal::forceVisitIgnored = false;
    memory::Factory::FreeAll();
    PrefabManager::GetInstance()->Clear();
    memory::Factory::Destroy(Engine::GetActiveScene());
    Engine::SetActiveScene(nullptr);
  }
}

void SceneLoader::SaveScene(Scene* scene, const char* filename) {
  xml_document<> doc;

  SerializationTypeVisitor visitor(scene, &doc);
  scene->GetReflection().Accept(&visitor);

  ReflectionHelper::ResolvePendingPointers();

  std::ofstream myfile;
  myfile.open(filename);
  myfile << doc;
  myfile.close();

  ReflectionHelper::ClearTrackedStrings();
}