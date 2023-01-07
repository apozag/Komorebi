#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml.hpp"

#include "TypeDescriptors.h"

namespace reflection {

  //--------------------------------------------------------
  // TypeDescriptor
  //--------------------------------------------------------
  const std::string TypeDescriptor::getFullName() const { return std::string(name); }

  
  //--------------------------------------------------------
  // TypeDescriptor_Struct
  //--------------------------------------------------------

  void TypeDescriptor_Struct::deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const {
    if (parentTypeDesc) parentTypeDesc->deserialize(obj, xmlNode);
    typedef rapidxml::xml_node<>* nodePtr;
    nodePtr child = xmlNode->first_node();
    while (child) {
      for (Member member : members) {
        if (!std::strcmp(child->name(), member.name)) {
          member.type->deserialize((char*)obj + member.offset, child);
          break;
        }
      }
      child = child->next_sibling();
    }
  };

  void TypeDescriptor_Struct::serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const {
    if (!varName) { varName = name; }
    rapidxml::xml_node<>* newNode = doc->allocate_node(rapidxml::node_type::node_element, varName);
    xmlParent->append_node(newNode);
    if (parentTypeDesc) serializeMembersVisit(parentTypeDesc, obj, newNode, doc);
    serializeMembers(obj, newNode, doc);
  };

  void TypeDescriptor_Struct::serializeMembers(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const {
    for (Member member : members) {
      member.type->serialize((char*)obj + member.offset, member.name, xmlNode, doc);
    }
  };

  //--------------------------------------------------------
  // TypeDescriptor_StdVector
  //--------------------------------------------------------

  const std::string TypeDescriptor_StdVector::getFullName() const {
    static std::string fullName = std::string("std::vector<") + itemType->getFullName() + ">";
    return fullName;
  }

  void TypeDescriptor_StdVector::deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const {
    typedef rapidxml::xml_node<>* nodePtr;
    size_t offset = 0;
    size_t count = 0;
    nodePtr child = xmlNode->first_node();
    while (child) {
      count++;
      child = child->next_sibling();
    }
    resize(obj, count);
    child = xmlNode->first_node();    
    while (child) {
      itemType->deserialize((char*)obj + offset, child);
      offset += itemType->size;
      child = child->next_sibling();
    }
  };

  void TypeDescriptor_StdVector::serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const {
    if (!varName) {
      std::string* name = new std::string(getFullName());
      ReflectionHelper::TrackString(name);
      varName = name->c_str();
    }
    typedef rapidxml::xml_node<>* nodePtr;
    nodePtr newNode = doc->allocate_node(rapidxml::node_type::node_element, varName);
    xmlParent->append_node(newNode);
    size_t size = getSize(obj);
    for (int i = 0; i < size; i++) {
      itemType->serialize(getItem(obj, i), nullptr, newNode, doc);
    }
  };

} // namespace reflect