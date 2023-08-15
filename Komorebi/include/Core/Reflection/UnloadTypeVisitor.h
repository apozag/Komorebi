#pragma once


#include "Core/Reflection/TypeVisitor.h"

namespace reflection {
  class UnloadTypeVisitor : public TypeVisitor {
  public:
    UnloadTypeVisitor(void* pObj) :
      m_pObj(pObj)
    {}
    void Visit(const TypeDescriptor* type) override {}
    void Visit(const TypeDescriptor_Struct* type) override;
    void Visit(const TypeDescriptor_StdVector* type) override;
    void Visit(const TypeDescriptor_Weak_Ptr* type) override {}
    void Visit(const TypeDescriptor_Owned_Ptr* type) override;
  private:

    void UnloadMembers(const TypeDescriptor_Struct* type);

    void* m_pObj;
  };
}
