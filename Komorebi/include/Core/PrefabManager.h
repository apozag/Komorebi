#pragma once

#include <vector>
#include <string>

#include "Core/Singleton.h"

#include "Core/Reflection/TypeDescriptors.h"

class GameObject;

class PrefabManager : public Singleton<PrefabManager>{
public:
  struct PrefabInfo {
    std::string m_name;
    std::string m_fileName;
    const reflection::TypeDescriptor* m_typeDesc;
    void* m_ptr;
    PrefabManager::PrefabInfo() {}
    PrefabManager::PrefabInfo(const std::string& name, const std::string& fileName, const reflection::TypeDescriptor* typeDesc, void* ptr) :
      m_name(name), m_fileName(fileName), m_typeDesc(typeDesc), m_ptr(ptr) {
    }
  };

public:
  
  template<class T>
  T* LoadPrefab(const char* filename, bool setup = true) {
    static const reflection::TypeDescriptor* typeDesc = reflection::TypeResolver<T>::get();
    void* pObj = nullptr;
    if (typeDesc->create != nullptr) {
      pObj = typeDesc->create();
      LoadPrefab(filename, pObj, typeDesc, setup);
      return (T*)pObj;
    }
    return nullptr;
  }
  
  template<class T>
  void SavePrefab(const char* filename, T* pObj) {
    SavePrefab(filename, pObj, reflection::TypeResolver<T>::get());
  }

  const PrefabManager::PrefabInfo& LoadPrefab(const char* filename, void* pObj, const reflection::TypeDescriptor* typeDesc, bool setup = true);
  void SavePrefab(const char* filename, void* pObj, const reflection::TypeDescriptor* typeDesc);

  std::vector<PrefabInfo>& GetAllLoadedPrefabs() { return m_loadedPrefabs; }

  template<class T>
  std::vector<PrefabInfo> GetLoadedPrefabs() {
    static const reflection::TypeDescriptor* typeDesc = reflection::TypeResolver<T>::get();
    return GetLoadedPrefabs(typeDesc);
  }

  std::vector<PrefabInfo> GetLoadedPrefabs(const reflection::TypeDescriptor* typeDesc);

  const PrefabManager::PrefabInfo* GetPrefabInfo(const char* filename);

private:  

  std::vector<std::string> m_loadingPrefabNameStack;
  std::vector<PrefabInfo> m_loadedPrefabs;
};