#pragma once

#include <string>
#include <unordered_map>

namespace reflection{
  class TypeDescriptor;
}

namespace rapidxml {
  template<class Ch = char>
  class xml_node;
}

class Scene;
class Node;

class SceneLoader {
public:

  static Scene* LoadScene(const char* filename);

  static void SaveScene(Scene* scene, const char* filename);
};