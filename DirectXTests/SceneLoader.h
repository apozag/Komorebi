#pragma once

#include <string>
#include <unordered_map>

namespace reflect{
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

  static void RegisterTypeDesc(reflect::TypeDescriptor* typeDesc);

  static Scene* LoadScene(const char* filename);

  static void SaveScene(Scene* scene, const char* filename);

private:

  static void ParseNode(rapidxml::xml_node<>* elemNode, Node* parent);

  static void SerializeNode(rapidxml::xml_node<>* elemNode, rapidxml::xml_document<>* doc, Node* node);

  static reflect::TypeDescriptor* GetTypeDesc(std::string name);

  typedef std::unordered_map<std::string, reflect::TypeDescriptor*> TypeDict;

  static TypeDict& GetTypeDict() {
    static TypeDict typeDict;
    return typeDict;
  };
};