#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "rapidxml.hpp"

namespace reflection {

  void DeserializationTypeVisitor::Visit(const TypeDescriptor* type) {
    type->SetValueFromString(m_pObj, m_xmlNode->value());
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Struct* type) {
    void* pObj = m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;
    if (type->parentTypeDesc) {
      type->parentTypeDesc->Accept(this);
    }
    type->construct(pObj);
    rapidxml::xml_node<>* child = xmlNode->first_node();
    int startMemberIdx = type->getFirstMemberIdx();
    int memberIdx = 0;
    child = xmlNode->first_node();
    // Fast forward to corresponding member index
    while (child && memberIdx < startMemberIdx) {
      child = child->next_sibling();
      memberIdx++;
    }
    // Actually deserialize members
    memberIdx = 0;
    while (child && memberIdx < type->members.size()) {
      m_pObj = type->members[memberIdx].getAddress(pObj);
      m_xmlNode = child;
      type->members[memberIdx].type->Accept(this);
      child = child->next_sibling();
      memberIdx++;
    }

    type->setup(pObj);

    int i = 0;
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
    ReflectionHelper::RegisterPendingPtr((void**)m_pObj, (unsigned int)std::atoi(m_xmlNode->value()));
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
    void** ppObj = (void**)m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;

    char* name = nullptr;
    unsigned int ptrId = 0xFFFFFFFF;

    rapidxml::xml_attribute<>* att = xmlNode->first_attribute();
    while (att) {
      if (strcmp(att->name(), __PtrIdAttName) == 0) {
        ptrId = (unsigned int)std::atoi(att->value());
      }
      else if (strcmp(att->name(), __PtrTypeAttName) == 0) {
        name = att->value();
      }
      att = att->next_attribute();
    }

    if (!name) {
      // TODO: [ERROR] Owned Pointer node must have a "Type" attribute
    }
    else if (ptrId == 0xFFFFFFFF) {
      // TODO: [ERROR] Owned Pointer node must have a "PrtId" attribute
    }

    const TypeDescriptor* typeDesc = ReflectionHelper::GetTypeDesc(name);

    *ppObj = malloc(typeDesc->size);

    ReflectionHelper::RegisterPtrId(*ppObj, ptrId, typeDesc->size);

    m_pObj = *ppObj;
    typeDesc->Accept(this);
  }

}