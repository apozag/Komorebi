#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml.hpp"

#include "ReflectionMacros.h"

IMPLEMENT_REFLECTION_PRIMITIVE(int, Int)
IMPLEMENT_REFLECTION_PRIMITIVE(unsigned int, UInt)
IMPLEMENT_REFLECTION_PRIMITIVE(uint64_t, UInt64)
IMPLEMENT_REFLECTION_PRIMITIVE(float, Float)
IMPLEMENT_REFLECTION_PRIMITIVE(double, Double)
IMPLEMENT_REFLECTION_PRIMITIVE(bool, Bool)

__IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(std::string*, CStr)
virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
  static_cast<std::string*>(obj)->assign(xmlNode->value());
};
virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override {
  rapidxml::xml_node<>* newNode = doc->allocate_node(rapidxml::node_type::node_element, varName); \
  xmlParent->append_node(newNode);  \
  newNode->value(static_cast<const std::string*>(obj)->c_str());
};
__IMPLEMENT_REFLECTION_PRIMITIVE_END(std::string, CStr)