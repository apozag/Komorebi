#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Core/PrefabManager.h"
#include "rapidxml.hpp"

namespace reflection {

  void DeserializationTypeVisitor::Visit(const TypeDescriptor* type) {
    type->SetValueFromString(m_pObj, m_xmlNode->value());
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Struct* type) {
    void* pObj = m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;
    type->construct(pObj);
    if (type->parentTypeDesc) {
      type->parentTypeDesc->Accept(this);
    }
    rapidxml::xml_node<>* child = xmlNode->first_node();

    //int startMemberIdx = type->getFirstMemberIdx();
    //int memberIdx = 0;
    //child = xmlNode->first_node();
    //// Fast forward to corresponding member index
    //while (child && memberIdx < startMemberIdx) {
    //  child = child->next_sibling();
    //  memberIdx++;
    //}
    
    // Actually deserialize members
    while (child) {
      for (const TypeDescriptor_Struct::Member& member : type->members) {
        if (strcmp(member.name, child->name()) == 0) {
          m_pObj = member.getAddress(pObj);
          m_xmlNode = child;
          member.type->Accept(this);
          break;
        }
      }
      child = child->next_sibling();
    }
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_StdVector* type) {
    void* pObj = m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;

    type->construct(pObj);

    size_t count = 0;
    rapidxml::xml_node<>* child = xmlNode->first_node();
    while (child) {
      count++;
      child = child->next_sibling();
    }
    type->resize(pObj, count);
    child = xmlNode->first_node();
    count = 0;
    while (child) {
      m_pObj = type->getItem(pObj, count);
      m_xmlNode = child;
      type->itemType->Accept(this);
      child = child->next_sibling();
      count++;
    }
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Weak_Ptr* type) {
    intptr_t id = std::atoll(m_xmlNode->value());
    ReflectionHelper::RegisterPendingPtr((void**)m_pObj, id);
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
    void** ppObj = (void**)m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;

    std::string name;
    intptr_t ptrId = std::numeric_limits<intptr_t>::max();

    rapidxml::xml_attribute<>* att = xmlNode->first_attribute();
    while (att) {
      if (strcmp(att->name(), __PtrIdAttName) == 0) {
        ptrId = std::atoll(att->value());
      }
      else if (strcmp(att->name(), __PtrTypeAttName) == 0) {
        name = att->value();
      }
      att = att->next_attribute();
    }

    if (name.empty()) {
      // TODO: [ERROR] Owned Pointer node must have a "Type" attribute
      name = type->getStaticType()->name;
    }

    const TypeDescriptor* typeDesc = ReflectionHelper::GetTypeDesc(name);

    *ppObj = typeDesc->create();

    if (ptrId == std::numeric_limits<intptr_t>::max()) {
      // TODO: [ERROR] Owned Pointer node must have a "PrtId" attribute
    }
    else {
      ReflectionHelper::RegisterPtrId(*ppObj, ptrId, typeDesc->size);
    }

    m_pObj = *ppObj;
    typeDesc->Accept(this);
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Asset_Ptr* type) {
    char* filename = m_xmlNode->value();
    if (filename[0] != '\0') {
      void** ppObj = type->getPPtr(m_pObj);
      const TypeDescriptor* dynamicType = type->GetDynamic(ppObj);      
      *ppObj = dynamicType->create();
      PrefabManager::GetInstance()->LoadPrefab(filename, *ppObj, dynamicType);
      type->setFilename(m_pObj, filename);
    }
  }

  //////////////////// SETUP TYPE VISITOR ///////////////////////

  void SetupTypeVisitor::Visit(const TypeDescriptor_Struct* type) {
    void* pObj = m_pObj;

    int memberCount = type->members.size();
    for (int i = 0; i < memberCount; i++) {
      m_pObj = type->members[i].getAddress(pObj);
      type->members[i].type->Accept(this);
    }

    type->setup(pObj);
  }

  void SetupTypeVisitor::Visit(const TypeDescriptor_StdVector* type) {
    void* pObj = m_pObj;
    size_t size = type->getSize(pObj);
    for (size_t i = 0; i < size; i++) {
      m_pObj = type->getItem(pObj, i);
      type->itemType->Accept(this);
    }
  }

  void SetupTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
    void** ppObj = (void**)m_pObj;   
    if (*ppObj == nullptr) {
      return;
    }
    const TypeDescriptor* dynamicType = type->GetDynamic(m_pObj);
    m_pObj = *ppObj;
    dynamicType->Accept(this);
  }

}