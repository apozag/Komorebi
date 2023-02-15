#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection//TypeVisitor.h"
#include "Core/GameObject.h"

namespace reflection {

  const std::string TypeDescriptor::getFullName() const { return std::string(name); }

  void TypeDescriptor::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Struct::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Owned_Ptr::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_Weak_Ptr::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  void TypeDescriptor_StdVector::Accept(TypeVisitor* visitor) const {
    visitor->Visit(this);
  }
  
  int TypeDescriptor_Struct::getFirstMemberIdx() const {
    if (!parentTypeDesc) return 0;
    return parentTypeDesc->getFirstMemberIdx() + parentTypeDesc->members.size();
  }

  const std::string TypeDescriptor_StdVector::getFullName() const {
    static std::string fullName = std::string("std::vector<") + itemType->getFullName() + ">";
    return fullName;
  }

} // namespace reflect