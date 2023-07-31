#pragma once

#include <string>
#include <unordered_map>

namespace rapidxml {
  template<class Ch = char>
  class xml_node;
  template<class Ch = char>
  class xml_document;
}

class Scene;
class Node;

class SceneLoader {
public:

  static Scene* LoadScene(const char* filename);
  static void UnloadScene();

  static void SaveScene(Scene* scene, const char* filename);

  static const std::string& GetLastLoadedFileName() { return m_lastLoadedFilename; }

private: 
  static std::string m_lastLoadedFilename;
};