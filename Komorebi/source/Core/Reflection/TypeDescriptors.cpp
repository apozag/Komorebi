#include <sstream>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection//TypeVisitor.h"
#include "Core/Reflection/ReflectionSettings.h"
#include "Core/GameObject.h"

namespace reflection {

  const std::string TypeDescriptor::getFullName() const { return std::string(name); }

  void TypeDescriptor::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Struct::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Enum::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Bitmask::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Owned_Ptr::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Weak_Ptr::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Asset_Ptr::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_StdVector::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Ignored::Accept(TypeVisitor* visitor) const {
    if (__internal::forceVisitIgnored) {
      visitor->Visit(this);
    }
  }
  
  int TypeDescriptor_Struct::getFirstMemberIdx() const {
    if (!parentTypeDesc) return 0;
    return parentTypeDesc->getFirstMemberIdx() + parentTypeDesc->members.size();
  }

  bool TypeDescriptor_Struct::CanUpCast(const TypeDescriptor_Struct* type) const {
    while (type != nullptr) {
      if (type == this) {
        return true;
      }
      type = type->parentTypeDesc;
    }
    return false;
  }

  std::string TypeDescriptor_Enum::GetValueStr(const void* obj) const {
    int val = *(int*)obj;
    for (int i = 0; i < values.size(); i++) {
      if (val == values[i].value) {
       return values[i].name;
      }
    }
    return "ERROR";
  }
  void TypeDescriptor_Enum::SetValueFromString(void* pObj, const char* valueCStr) const {
    for (int i = 0; i < values.size(); i++) {
      if (strcmp(valueCStr, values[i].name) == 0) {
        *(int*)pObj = values[i].value;
        return;
      }
    }
    *(int*)pObj = 0;
  }

  std::string TypeDescriptor_Bitmask::GetValueStr(const void* obj) const {
    std::string str;
    unsigned int bitmask = *(unsigned int*) obj;
    for (const TypeDescriptor_Enum::EnumValue& val : enumType->values) {
      if ((bitmask & val.value) != 0) {
        if (!str.empty()) {
          str += "|";
        }
        str += val.name;
      }
      else if (bitmask == 0 && val.value == 0) {
        str = val.name;
        break;
      }
    }    
    return str;
  }
  void TypeDescriptor_Bitmask::SetValueFromString(void* pObj, const char* valueCStr) const {
    *(unsigned int*)pObj = 0u;
    std::stringstream sstr(valueCStr);
    std::string subStr;
    while (std::getline(sstr, subStr, '|')) {
      for (const TypeDescriptor_Enum::EnumValue& val : enumType->values) {
        if (subStr == val.name) {
          *(unsigned int*)pObj |= val.value;
        }
      }
    }
  }

  const std::string TypeDescriptor_StdVector::getFullName() const {
    static std::string fullName = std::string("std::vector<") + itemType->getFullName() + ">";
    return fullName;
  }

  const TypeDescriptor* TypeDescriptor_Ptr::GetDynamic(const void* pObj) const {
    void** ppObj = (void**)pObj;
    return getDynamicType(*ppObj);
  }

} // namespace reflect