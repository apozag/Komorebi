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

  static void SaveScene(Scene* scene, const char* filename);

private:

  void ParseNode(rapidxml::xml_node<>* elemNode,  Node* parent);
  void SerializeNode(rapidxml::xml_node<>* elemNode, rapidxml::xml_document<>* doc, Node* node);
};