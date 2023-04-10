#pragma once

#include "Core/Reflection/TypeDescriptors.h"

namespace reflection {
  class TypeVisitor {
  public:
    virtual void Visit(const TypeDescriptor* type) = 0;
    virtual void Visit(const TypeDescriptor_Struct* type) = 0;
    virtual void Visit(const TypeDescriptor_StdVector* type) = 0;
    virtual void Visit(const TypeDescriptor_Weak_Ptr* type) = 0;
    virtual void Visit(const TypeDescriptor_Owned_Ptr* type) = 0;
    virtual void Visit(const TypeDescriptor_Enum* type) { this->Visit((TypeDescriptor*)type); };
    virtual void Visit(const TypeDescriptor_Bitmask* type) { this->Visit((TypeDescriptor*)type); };
  protected:
    TypeVisitor() {}
  };   

}