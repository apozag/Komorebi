#pragma once

#include "GUI/ImGuiTypeVisitor.h"

class ImGuiEditPtrTypeVisitor : public ImGuiTypeVisitor {
public:
  ImGuiEditPtrTypeVisitor(void* pObj, std::string name) : ImGuiTypeVisitor(pObj, name) {}
  virtual void Visit(const reflection::TypeDescriptor_Weak_Ptr* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Owned_Ptr* type) override;
  virtual void Visit(const reflection::TypeDescriptor_Asset_Ptr* type) override;
};
