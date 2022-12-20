#include"Reflection.h"

IMPLEMENT_REFLECTION_PRIMITIVE(int, Int)
IMPLEMENT_REFLECTION_PRIMITIVE(unsigned int, UInt)
IMPLEMENT_REFLECTION_PRIMITIVE(uint64_t, UInt64)
IMPLEMENT_REFLECTION_PRIMITIVE(float, Float)
IMPLEMENT_REFLECTION_PRIMITIVE(double, Double)
IMPLEMENT_REFLECTION_PRIMITIVE(bool, Bool)

__IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(std::string*, CStr)
virtual void dump(const void* obj, std::ostream& os, int /* unused */) const override {
  os << "CStr {\"" << static_cast<const std::string*>(obj)->c_str() << "\"}";
}
virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
  static_cast<std::string*>(obj)->assign(xmlNode->value());
};
virtual void serialize(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* /* unused */) const override {
  xmlNode->value(static_cast<const std::string*>(obj)->c_str());
};
__IMPLEMENT_REFLECTION_PRIMITIVE_END(std::string, CStr)
