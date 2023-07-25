#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/ReflectionImplMacros.h"

IMPLEMENT_REFLECTION_PRIMITIVE(int, Int)
IMPLEMENT_REFLECTION_PRIMITIVE(unsigned int, UInt)
IMPLEMENT_REFLECTION_PRIMITIVE(uint64_t, UInt64)
IMPLEMENT_REFLECTION_PRIMITIVE(float, Float)
IMPLEMENT_REFLECTION_PRIMITIVE(double, Double)
//IMPLEMENT_REFLECTION_PRIMITIVE(bool, Bool)

__IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(bool, Bool)
std::string GetValueStr(const void* obj) const override {
  if(!obj) return std::string();
  bool value = *(bool*)obj;
  return value ? "true" : "false";
}
void SetValueFromString(void* pObj, const char* valueCStr) const override {
  (*(bool*)pObj) = strcmp(valueCStr, "true") == 0 || strcmp(valueCStr, "1") == 0;
}
__IMPLEMENT_REFLECTION_PRIMITIVE_END(bool, Bool)

__IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(std::string, CStr)
std::string GetValueStr(const void* obj) const override{  
  return obj? *static_cast<const std::string*>(obj) : std::string();
} 
void SetValueFromString(void* pObj, const char* valueCStr) const override {
  (*(std::string*)pObj) = valueCStr; 
} 
__IMPLEMENT_REFLECTION_PRIMITIVE_END(std::string, CStr)



IMPLEMENT_REFLECTION_VECTOR(std::string)
IMPLEMENT_REFLECTION_VECTOR(float)
