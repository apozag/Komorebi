#pragma once

#include "Core/Reflection/TypeVisitor.h"

namespace rapidxml {
  template<class Ch = char>
  class xml_node;
  template<class Ch = char>
  class xml_document;
}

namespace reflection {
  class SerializationTypeVisitor : public TypeVisitor {
  public:
    SerializationTypeVisitor(const void* pObj, rapidxml::xml_document<>* doc) :
      m_pObj(pObj),
      m_doc(doc),
      m_xmlParent((rapidxml::xml_node<>*)doc)
    {}
    void Visit(const TypeDescriptor* type) override;
    void Visit(const TypeDescriptor_Struct* type) override;
    void Visit(const TypeDescriptor_StdVector* type) override;
    void Visit(const TypeDescriptor_Weak_Ptr* type) override;
    void Visit(const TypeDescriptor_Owned_Ptr* type) override;
  private:
    void SerializeMembers(const TypeDescriptor_Struct* type);
  private:
    const void* m_pObj;
    const char* m_varName = nullptr;
    rapidxml::xml_node<>* m_xmlParent;
    rapidxml::xml_document<>* m_doc;
  };
}