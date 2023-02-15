#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/ReflectionMacros.h"

IMPLEMENT_REFLECTION_PRIMITIVE(int, Int)
IMPLEMENT_REFLECTION_PRIMITIVE(unsigned int, UInt)
IMPLEMENT_REFLECTION_PRIMITIVE(uint64_t, UInt64)
IMPLEMENT_REFLECTION_PRIMITIVE(float, Float)
IMPLEMENT_REFLECTION_PRIMITIVE(double, Double)
IMPLEMENT_REFLECTION_PRIMITIVE(bool, Bool)

__IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(std::string*, CStr)
std::string GetValueStr(const void* obj) const override{  
  return *static_cast<const std::string*>(obj);
} 
void SetValueFromString(void* pObj, const char* valueCStr) const override {
  *(std::string*)pObj = valueCStr; 
} 
__IMPLEMENT_REFLECTION_PRIMITIVE_END(std::string, CStr)