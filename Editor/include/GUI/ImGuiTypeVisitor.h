#pragma once

#include "Core/Reflection/TypeVisitor.h"

class ImGuiTypeVisitor : public reflection::TypeVisitor {
public:
  ImGuiTypeVisitor(void* pObj, std::string name) : m_pObj(pObj), m_currName(name) {}
  virtual void Visit(const reflection::TypeDescriptor* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Struct* type) override;
  virtual void Visit(const reflection::TypeDescriptor_StdVector* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Weak_Ptr* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Owned_Ptr* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Asset_Ptr* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Enum* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Bitmask* type) override;

protected:

  void VisitMembers(const reflection::TypeDescriptor_Struct* type, bool& currDirty);

  void* m_pObj;
  std::string m_currName;
  bool m_dirty = false;
};
