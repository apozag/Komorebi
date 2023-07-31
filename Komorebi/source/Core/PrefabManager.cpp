#include "Core/PrefabManager.h"

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/SerializationTypeVisitor.h"
#include "Core/Reflection/UnloadTypeVisitor.h"
#include "Core/Reflection/CopyTypeVisitor.h"
#include "Core/Reflection/ReflectionSettings.h"
#include "Core/Memory/Factory.h"

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

void* PrefabManager::LoadPrefab(const char* filename, const reflection::TypeDescriptor* typeDesc, bool setup) {

  if (typeDesc->create == nullptr) {
    return nullptr;
  }

  std::string strFilename = filename;
  const std::string& typeName = typeDesc->getFullName();

  void* pObj = typeDesc->create();

  // Return same instance if already loaded
  for (PrefabInfo& prefab : m_GlobalLoadedPrefabs) {
    if (prefab.m_fileName == strFilename && prefab.m_typeDesc == typeDesc) {
      return prefab.m_ptr;
    }
  }
  if (!memory::Factory::IsCurrentModeGlobal()) {
    for (PrefabInfo& prefab : m_TransientLoadedPrefabs) {
      if (prefab.m_fileName == strFilename && prefab.m_typeDesc == typeDesc) {
        //if (memory::Factory::IsCurrentModeGlobal()) {
        //  void* pObj = prefab.m_typeDesc->create();
        //  reflection::CopyTypeVisitor copyVisitor(pObj, prefab.m_ptr);
        //  prefab.m_typeDesc->Accept(&copyVisitor);
        //}
        //else {
        return prefab.m_ptr;
        //}
      }
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

  std::vector<PrefabInfo>& prefabs = memory::Factory::IsCurrentModeGlobal() ? m_GlobalLoadedPrefabs : m_TransientLoadedPrefabs;
  prefabs.push_back({ getFileNameFromPath(strFilename), strFilename, typeDesc, pObj });

  return pObj;
}

void PrefabManager::SavePrefab(const char* filename, void* pObj, const reflection::TypeDescriptor* typeDesc) {
  std::string strFilename = filename;
  const std::string& typeName = typeDesc->getFullName();

  bool alreadyLoaded = false;  

  std::vector<PrefabInfo>& prefabs = memory::Factory::IsCurrentModeGlobal() ? m_GlobalLoadedPrefabs : m_TransientLoadedPrefabs;
  auto it = prefabs.begin();
  for (; it != prefabs.end(); it++) {
    if (it->m_fileName == strFilename && it->m_typeDesc == typeDesc) {
      alreadyLoaded = true;
      if (it->m_ptr != pObj) {
        // TODO: [ERROR] To overwrite an asset, use the loaded object assigned to that filename (The original loaded object).
        return;
      }
      break;
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

  if (!alreadyLoaded) {
    prefabs.push_back({ getFileNameFromPath(filename), filename, typeDesc, pObj });
  }
}

std::vector<PrefabManager::PrefabInfo> PrefabManager::GetLoadedPrefabs(const reflection::TypeDescriptor* typeDesc) {
  std::vector<PrefabInfo> prefabs;
  for (const PrefabManager::PrefabInfo& prefab : m_GlobalLoadedPrefabs) {
    if (prefab.m_typeDesc == typeDesc) {
      prefabs.push_back(prefab);
    }
  }
  for (const PrefabManager::PrefabInfo& prefab : m_TransientLoadedPrefabs) {
    if (prefab.m_typeDesc == typeDesc) {
      prefabs.push_back(prefab);
    }
  }
  return prefabs;
}

std::vector<PrefabManager::PrefabInfo>& PrefabManager::GetAllLoadedPrefabs() {
  std::vector<PrefabInfo> prefabs;
  for (const PrefabManager::PrefabInfo& prefab : m_GlobalLoadedPrefabs) {
      prefabs.push_back(prefab);
  }
  for (const PrefabManager::PrefabInfo& prefab : m_TransientLoadedPrefabs) {
      prefabs.push_back(prefab);
  }
  return prefabs;
}

const PrefabManager::PrefabInfo* PrefabManager::GetPrefabInfo(const char* filename) {
  for (PrefabInfo& prefab : m_GlobalLoadedPrefabs) {
    if (prefab.m_fileName == filename) {
      return &prefab;
    }
  }
  for (PrefabInfo& prefab : m_TransientLoadedPrefabs) {
    if (prefab.m_fileName == filename) {
      return &prefab;
    }
  }
  return nullptr;
}

void PrefabManager::Clear() {
  reflection::__internal::forceVisitIgnored = true;
  for (PrefabInfo& prefab : m_TransientLoadedPrefabs) {
    reflection::UnloadTypeVisitor unloadVisitor(prefab.m_ptr);
    prefab.m_typeDesc->Accept(&unloadVisitor);
    // The prefab type is struct and not pointer, so it doesnt call destroy. We call it here.
    prefab.m_typeDesc->destroy(prefab.m_ptr);
  }
  reflection::__internal::forceVisitIgnored = false;
  m_TransientLoadedPrefabs.clear();
}
