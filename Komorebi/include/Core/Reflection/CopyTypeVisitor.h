#pragma once

#include "Core/Reflection/TypeVisitor.h"

namespace reflection {

  class CopyTypeVisitor : public reflection::TypeVisitor {
  public:
    CopyTypeVisitor(void* pObjDst, void* pObjSrc) : m_pObjDst(pObjDst), m_pObjSrc(pObjSrc) {}
    void Visit(const reflection::TypeDescriptor* type);
    void Visit(const reflection::TypeDescriptor_Struct* type);
    void Visit(const reflection::TypeDescriptor_StdVector* type);
    void Visit(const reflection::TypeDescriptor_Weak_Ptr* type) {}
    void Visit(const reflection::TypeDescriptor_Owned_Ptr* type);
  private:
    void* m_pObjDst;
    void* m_pObjSrc;
  };
}
