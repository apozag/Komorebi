#pragma once

#include "Core/Reflection/UnloadTypeVisitor.h"

namespace reflection {

  void UnloadTypeVisitor::Visit(const TypeDescriptor_Struct* type) {
    void* pObj = m_pObj;

    int memberCount = type->members.size();
    for (int i = 0; i < memberCount; i++) {
      m_pObj = type->members[i].getAddress(pObj);
      type->members[i].type->Accept(this);
    }

    type->destroy(pObj);
  }

  void UnloadTypeVisitor::Visit(const TypeDescriptor_StdVector* type) {
    void* pObj = m_pObj;
    size_t size = type->getSize(pObj);
    for (size_t i = 0; i < size; i++) {
      m_pObj = type->getItem(pObj, i);
      type->itemType->Accept(this);
    }
  }

  void UnloadTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
    void** ppObj = (void**)m_pObj;
    const TypeDescriptor* dynamicType = type->GetDynamic(m_pObj);
    m_pObj = *ppObj;
    dynamicType->Accept(this);    
  }

}