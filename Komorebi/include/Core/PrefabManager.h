#pragma once

#include <vector>
#include <string>

#include "Core/Singleton.h"

#include "Core/Reflection/TypeDescriptors.h"

//namespace reflection {
//  class TypeDescriptor;
//
//  template<class T, typename Enable>
//  class TypeResolver;
//}

class GameObject;

struct PrefabInfo {
  std::string m_name;
  std::string m_fileName;
  const reflection::TypeDescriptor* m_typeDesc;
  void* m_ptr;
  PrefabInfo() {}
  PrefabInfo(const std::string& name, const std::string& fileName, const reflection::TypeDescriptor* typeDesc, void* ptr) :
    m_name(name), m_fileName(fileName), m_typeDesc(typeDesc), m_ptr(ptr) 
  {}
};

class PrefabManager : public Singleton<PrefabManager>{
public:
  template<class T>
  T* LoadPrefab(const char* filename, bool setup = true) {
    const reflection::TypeDescriptor* typeDesc = reflection::TypeResolver<OWNED_PTR(T)>::get();
    void* pObj = typeDesc->create();
    LoadPrefab(filename, pObj, typeDesc, setup);
    return (T*)pObj;
  }

  template<class T>
  void SavePrefab(const char* filename, T* pObj) {
    SavePrefab(filename, pObj, reflection::TypeResolver<T>::get());
  }

  const PrefabInfo& LoadPrefab(const char* filename, void* pObj, const reflection::TypeDescriptor* typeDesc, bool setup = true);
  void SavePrefab(const char* filename, void* pObj, const reflection::TypeDescriptor* typeDesc);

  std::vector<PrefabInfo>& GetLoadedPrefabs() { return m_loadedPrefabs; }

  const PrefabInfo* GetPrefabInfo(const char* filename);

private:  

  std::vector<std::string> m_loadingPrefabNameStack;
  std::vector<PrefabInfo> m_loadedPrefabs;
};