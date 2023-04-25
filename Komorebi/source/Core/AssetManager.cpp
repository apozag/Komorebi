#include "Core/AssetManager.h"

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/SerializationTypeVisitor.h"

template<class T>
T* AssetManager::LoadAsset(const char* filename, bool setup) {

  const reflection::TypeDescriptor* typeDesc = reflection::TypeResolver<OWNED_PTR(T)>::get();

  std::string strFilename = filename;
  std::string& typeName = typeDesc->getFullName();

  // Return same instance if already loaded
  auto it = m_loadedAssets.begin();
  for (; it != m_loadedAssets.end(); it++) {
    if (it->name == strFilename && it->typeName == typeName) {
      return (T*)(it->ptr);
    }
  }

  if (std::find(m_loadingAssetNameStack.begin(), m_loadingAssetNameStack.end(), strFilename) != m_loadingAssetNameStack.end()) {
    // TODO: [ERROR] Circular asset dependency
    return nullptr;
  }

  // Load asset from xml
  rapidxml::file<> xmlFile(filename);
  rapidxml::xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  const rapidxml::xml_node<>* nodeElem = doc.first_node(typeName);

  m_loadingAssetNameStack.push_back(strFilename);

  T* pObj = nullptr;  

  reflection::DeserializationTypeVisitor deserializationVisitor(&pObj, nodeElem);
  typeDesc->Accept(&deserializationVisitor);

  reflection::ReflectionHelper::ResolvePendingPointers();

  if (setup) {
    reflection::SetupTypeVisitor setupVisitor(&pObj);
    typeDesc->Accept(setupVisitor);
  }

  m_loadedAssets({strFilename, typeName, pObj});

  m_loadingAssetNameStack.pop_back();

  return pObj;
}

template<class T>
void AssetManager::SaveAsset(const char* filename, T* pObj) {
  std::string strFilename = filename;
  std::string& typeName = reflection::TypeResolver<T>::get()->getFullName();

  auto it = m_loadedAssets.begin();
  for (; it != m_loadedAssets.end(); it++) {
    if (it->name == strFilename && it->typeName == typeName && it->ptr != pObj) {
      // TODO: [ERROR] To overwrite an asset, use the loaded object assigned to that filename. This is for avoiding memory leaks.
      return;
    }
  }

  rapidxml::xml_document<> doc;
  reflection::TypeResolver<OWNED_PTR(T)>::get()->serialize(pObj, filename, &doc, &doc);

  std::ofstream myfile;
  myfile.open(filename);
  myfile << doc;
  myfile.close();

  reflection::ReflectionHelper::ClearTrackedStrings();
}
