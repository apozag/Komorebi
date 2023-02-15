#pragma once

#include "Core/Reflection/TypeDescriptors.h"

namespace reflection {

  //class TypeDescriptor;
  //class TypeDescriptor_Struct;
  //class TypeDescriptor_StdVector;
  //class TypeDescriptor_Weak_Ptr;
  //class TypeDescriptor_Owned_Ptr;


  class TypeVisitor {
  public:
    virtual void Visit(const TypeDescriptor* type) = 0;
    virtual void Visit(const TypeDescriptor_Struct* type) = 0;
    virtual void Visit(const TypeDescriptor_StdVector* type) = 0;
    virtual void Visit(const TypeDescriptor_Weak_Ptr* type) = 0;
    virtual void Visit(const TypeDescriptor_Owned_Ptr* type) = 0;
  protected:
    TypeVisitor() {}
  };   

} // namespoace reflection