#pragma once

#include "Core/Reflection/CopyTypeVisitor.h"

#include "Core/Memory/Factory.h"
#include "Core/PrefabManager.h"

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
    void* pObjSrc = m_pObjSrc;
    void* pObjDst = m_pObjDst;

    size_t size = type->getSize(pObjSrc);
    type->resize(pObjDst, size);
    for (size_t i = 0; i < size; i++) {
      m_pObjSrc = type->getItem(pObjSrc, i);
      m_pObjDst = type->getItem(pObjDst, i);
      type->itemType->Accept(this);
    }
  }

  void CopyTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
    void** ppObjSrc = (void**)m_pObjSrc;
    void** ppObjDst = (void**)m_pObjDst;

    const TypeDescriptor* dynamicTypeSrc = type->GetDynamic(ppObjSrc);

    // TypeDescriptor_Ignored does not have these function pointers assigned
    if (dynamicTypeSrc->create == nullptr) {
      return;
    }

    if (*ppObjDst != nullptr) {
      const TypeDescriptor* dynamicTypeDst= type->GetDynamic(ppObjDst);
      // TODO: This is a memory leak, think of something
      if (dynamicTypeDst->destroy) {
        dynamicTypeDst->destroy(*ppObjDst);
      }
    }

    *ppObjDst = dynamicTypeSrc->create();

    m_pObjSrc = *ppObjSrc;
    m_pObjDst = *ppObjDst;
    dynamicTypeSrc->Accept(this);
  }

  void CopyTypeVisitor::Visit(const reflection::TypeDescriptor_Asset_Ptr* type) {
    void** ppObjSrc = type->getPPtr(m_pObjSrc);
    void** ppObjDst = type->getPPtr(m_pObjDst);
    const TypeDescriptor* dynamicType = type->GetDynamic(ppObjSrc);
    *ppObjDst = dynamicType->create();
    std::string filename = type->getFilename(m_pObjSrc);
    PrefabManager::GetInstance()->LoadPrefab(filename.c_str(), *ppObjDst, dynamicType);
    type->setFilename(m_pObjDst, filename);
  }

}