#pragma once

#include <unordered_map>
#include <string>

#include "Core/Singleton.h"

class GameObject;

struct AssetInfo {
  std::string name;
  std::string typeName;
  void* ptr;
};

class AssetManager : public Singleton<AssetManager>{
  template<class T>
  T* LoadAsset(const char* filename, bool setup = true);
  template<class T>
  void SaveAsset(const char* filename, T* pObj);
private:
  std::vector<std::string> m_loadingAssetNameStack;
  std::vector<AssetInfo> m_loadedAssets;
};