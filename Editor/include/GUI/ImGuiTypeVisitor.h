#pragma once

#include "Core/Reflection/TypeVisitor.h"

class ImGuiTypeVisitor : public reflection::TypeVisitor {
public:
  ImGuiTypeVisitor(void* pObj) : m_pObj(pObj) {}
  void Visit(const reflection::TypeDescriptor* type);
  void Visit(const reflection::TypeDescriptor_Struct* type);
  void Visit(const reflection::TypeDescriptor_StdVector* type);
  void Visit(const reflection::TypeDescriptor_Weak_Ptr* type);
  void Visit(const reflection::TypeDescriptor_Owned_Ptr* type);
private:
  void* m_pObj;
  bool m_dirty = false;
};
