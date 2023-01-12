#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml.hpp"

#include "TypeDescriptors.h"
#include "GameObject.h"

namespace reflection {

  //--------------------------------------------------------
  // TypeDescriptor
  //--------------------------------------------------------
  const std::string TypeDescriptor::getFullName() const { return std::string(name); }

  
  //--------------------------------------------------------
  // TypeDescriptor_Struct
  //--------------------------------------------------------

  void TypeDescriptor_Struct::deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const {
    if (parentTypeDesc) { 
      parentTypeDesc->deserialize(obj, xmlNode); 
    }
    construct(obj);
    rapidxml::xml_node<>* child = xmlNode->first_node();
    int startMemberIdx = getFirstMemberIdx();
    int memberIdx = 0;
    child = xmlNode->first_node();
    // Fast forward to corresponding member index
    while (child && memberIdx < startMemberIdx) {
      child = child->next_sibling();
      memberIdx++;
    }
    // Actually deserialize members
    memberIdx = 0;
    while (child && memberIdx < members.size()) {
      /*for (const Member& member : members) {
        if (!std::strcmp(child->name(), member.name)) {
          member.type->deserialize((char*)obj + member.offset, child);
          break;
        }
      }*/
      members[memberIdx].type->deserialize((char*)obj + members[memberIdx].offset, child);
      child = child->next_sibling();
      memberIdx++;
    }

    setup(obj);
  };

  void TypeDescriptor_Struct::serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const {
    if (!varName) { varName = name; }
    rapidxml::xml_node<>* newNode = doc->allocate_node(rapidxml::node_type::node_element, varName);
    xmlParent->append_node(newNode);
    serializeMembers(obj, newNode, doc);
  };

  void TypeDescriptor_Struct::serializeMembers(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const {
    if (parentTypeDesc) serializeMembersVisit(parentTypeDesc, obj, xmlNode, doc);
    for (const Member& member : members) {
      member.type->serialize((char*)obj + member.offset, member.name, xmlNode, doc);
    }
  };

  int TypeDescriptor_Struct::getFirstMemberIdx() const {
    if (!parentTypeDesc) return 0;
    TypeDescriptor_Struct* parent = ((TypeDescriptor_Struct*)parentTypeDesc);
    return parent->getFirstMemberIdx() + parent->members.size();
  }

  //--------------------------------------------------------
  // TypeDescriptor_StdVector
  //--------------------------------------------------------

  const std::string TypeDescriptor_StdVector::getFullName() const {
    static std::string fullName = std::string("std::vector<") + itemType->getFullName() + ">";
    return fullName;
  }

  void TypeDescriptor_StdVector::deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const {
    construct(obj);
    size_t count = 0;
    rapidxml::xml_node<>* child = xmlNode->first_node();
    while (child) {
      count++;
      child = child->next_sibling();
    }
    resize(obj, count);
    child = xmlNode->first_node();    
    count = 0;
    while (child) {
      itemType->deserialize(getItem(obj, count), child);
      child = child->next_sibling();
      count++;
    }
  };

  void TypeDescriptor_StdVector::serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const {
    if (!varName) {
      std::string* name = new std::string(getFullName());
      ReflectionHelper::TrackString(name);
      varName = name->c_str();
    }
    rapidxml::xml_node<>* newNode = doc->allocate_node(rapidxml::node_type::node_element, varName);
    xmlParent->append_node(newNode);
    size_t size = getSize(obj);
    for (int i = 0; i < size; i++) {
      itemType->serialize(getItem(obj, i), nullptr, newNode, doc);
    }
  };

} // namespace reflect