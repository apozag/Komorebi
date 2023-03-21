#pragma once

#include "Core/Reflection/CopyTypeVisitor.h"

namespace reflection {

  void CopyTypeVisitor::Visit(const reflection::TypeDescriptor* type) {
    if (strcmp(type->name, "std::string") == 0) {
      std::string* strDst = (std::string*)m_pObjDst;
      std::string* strSrc = (std::string*)m_pObjSrc;
      *strDst = *strSrc;
    }
    else {
      memcpy(m_pObjDst, m_pObjSrc, type->size);
    }
  }

  void CopyTypeVisitor::Visit(const TypeDescriptor_Struct* type) {
    void* pObjSrc = m_pObjSrc;
    void* pObjDst = m_pObjDst;

    size_t memberCount = type->members.size();
    for (size_t i = 0; i < memberCount; i++) {
      m_pObjDst = type->members[i].getAddress(pObjDst);
      m_pObjSrc = type->members[i].getAddress(pObjSrc);
      type->members[i].type->Accept(this);
    }
  }

  void CopyTypeVisitor::Visit(const TypeDescriptor_StdVector* type) {
    /*void* pObjSrc = m_pObjSrc;
    void* pObjDst = m_pObjDst;

    size_t size = type->getSize(pObj);
    for (size_t i = 0; i < size; i++) {
      m_pObj = type->getItem(pObj, i);
      type->itemType->Accept(this);
    }*/
  }

  void CopyTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
   /* void* pObjSrc = m_pObjSrc;
    void* pObjDst = m_pObjDst;

    const TypeDescriptor* dynamicType = type->GetDynamic(m_pObj);
    m_pObj = *ppObj;
    dynamicType->Accept(this);
    delete(*ppObj);*/
  }

}