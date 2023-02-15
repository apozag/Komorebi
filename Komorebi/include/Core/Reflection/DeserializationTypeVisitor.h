#pragma once

#include "Core/Reflection/TypeVisitor.h"

namespace rapidxml {
  template<class Ch = char>
  class xml_node;
}

namespace reflection {
  class DeserializationTypeVisitor : public TypeVisitor {
  public:
    DeserializationTypeVisitor(void* pObj, const rapidxml::xml_node<>* xmlNode) :
      m_pObj(pObj),
      m_xmlNode(xmlNode)
    {}
    void Visit(const TypeDescriptor* type) override;
    void Visit(const TypeDescriptor_Struct* type) override;
    void Visit(const TypeDescriptor_StdVector* type) override;
    void Visit(const TypeDescriptor_Weak_Ptr* type) override;
    void Visit(const TypeDescriptor_Owned_Ptr* type) override;
  private:
    void* m_pObj;
    const rapidxml::xml_node<>* m_xmlNode;
  };
}