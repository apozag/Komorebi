#include "Core/PrefabManager.h"

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/SerializationTypeVisitor.h"

std::string getFileNameFromPath(const std::string& path) {
  size_t idx = path.npos;
  if (idx == path.npos) {
    idx = path.find_last_of("/\\");
  }
  if (idx == path.npos) {
    idx = path.find_last_of("\\");
  }
  if (idx == path.npos) {
    idx = path.find_last_of("\\");
  }
  if (idx == path.npos) {
    idx = path.find_last_of("/");
  }
  if (idx == path.npos) {
    idx = path.find_last_of("\/");
  }
  std::string auxStr = path.substr(idx + 1);
  return auxStr.substr(0, auxStr.find_last_of('.'));
}

const PrefabManager::PrefabInfo& PrefabManager::LoadPrefab(const char* filename, void* pObj, const reflection::TypeDescriptor* typeDesc, bool setup) {
  std::string strFilename = filename;
  const std::string& typeName = typeDesc->getFullName();

  // Return same instance if already loaded
  for (PrefabInfo& prefab : m_loadedPrefabs) {
    if (prefab.m_name == strFilename && prefab.m_typeDesc == typeDesc) {
      return prefab;
    }
  }

  if (std::find(m_loadingPrefabNameStack.begin(), m_loadingPrefabNameStack.end(), strFilename) != m_loadingPrefabNameStack.end()) {
    // TODO: [ERROR] Circular asset dependency
    return {};
  }

  // Load asset from xml
  rapidxml::file<> xmlFile(filename);
  rapidxml::xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  const rapidxml::xml_node<>* nodeElem = doc.first_node(typeName.c_str());

  m_loadingPrefabNameStack.push_back(strFilename);

  reflection::DeserializationTypeVisitor deserializationVisitor(pObj, nodeElem);
  typeDesc->Accept(&deserializationVisitor);

  reflection::ReflectionHelper::ResolvePendingPointers();

  if (setup) {
    reflection::SetupTypeVisitor setupVisitor(pObj);
    typeDesc->Accept(&setupVisitor);
  } 

  m_loadingPrefabNameStack.pop_back();

  m_loadedPrefabs.push_back({ getFileNameFromPath(strFilename), strFilename, typeDesc, pObj });

  return m_loadedPrefabs[m_loadedPrefabs.size() - 1];
}

void PrefabManager::SavePrefab(const char* filename, void* pObj, const reflection::TypeDescriptor* typeDesc) {
  std::string strFilename = filename;
  const std::string& typeName = typeDesc->getFullName();

  auto it = m_loadedPrefabs.begin();
  for (; it != m_loadedPrefabs.end(); it++) {
    if (it->m_name == strFilename) { // && it->m_typeDesc == typeDesc && it->m_ptr != pObj) {
      // TODO: [ERROR] To overwrite an asset, use the loaded object assigned to that filename. This is for avoiding memory leaks.
      // TODO: wtf is the above commentary
      return;
    }
  }

  rapidxml::xml_document<> doc;
  reflection::SerializationTypeVisitor visitor(pObj, &doc);
  typeDesc->Accept(&visitor);

  std::ofstream myfile;
  myfile.open(filename);
  myfile << doc;
  myfile.close();

  reflection::ReflectionHelper::ClearTrackedStrings();

  m_loadedPrefabs.push_back({ getFileNameFromPath(filename), filename, typeDesc, pObj });
}

std::vector<PrefabManager::PrefabInfo> PrefabManager::GetLoadedPrefabs(const reflection::TypeDescriptor* typeDesc) {
  std::vector<PrefabInfo> prefabs;
  for (const PrefabManager::PrefabInfo& prefab : m_loadedPrefabs) {
    if (prefab.m_typeDesc == typeDesc) {
      prefabs.push_back(prefab);
    }
  }
  return prefabs;
}

const PrefabManager::PrefabInfo* PrefabManager::GetPrefabInfo(const char* filename) {
  for (PrefabInfo& prefab : m_loadedPrefabs) {
    if (prefab.m_fileName == filename) {
      return &prefab;
    }
  }
  return nullptr;
}

