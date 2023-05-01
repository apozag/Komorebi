#pragma once

#include "Core/Reflection/TypeVisitor.h"

class ImGuiTypeVisitor : public reflection::TypeVisitor {
public:
  ImGuiTypeVisitor(void* pObj) : m_pObj(pObj) {}
  void Visit(const reflection::TypeDescriptor* type) override;
  void Visit(const reflection::TypeDescriptor_Struct* type) override;
  void Visit(const reflection::TypeDescriptor_StdVector* type) override;
  void Visit(const reflection::TypeDescriptor_Weak_Ptr* type) override;
  void Visit(const reflection::TypeDescriptor_Owned_Ptr* type) override;
  void Visit(const reflection::TypeDescriptor_Asset_Ptr* type) override;
  void Visit(const reflection::TypeDescriptor_Enum* type) override;
  void Visit(const reflection::TypeDescriptor_Bitmask* type) override;
private:
  void* m_pObj;
  bool m_dirty = false;
};
