#include "GUI/ImGuiTypeVisitor.h"

#include <string>

#include "imgui/imgui.h"

using namespace reflection;

void ImGuiTypeVisitor::Visit(const TypeDescriptor* type) {
  constexpr const size_t buffSize = 64;
  std::string stdStr = type->GetValueStr(m_pObj);
  char str[buffSize];
  memcpy(str, stdStr.c_str(), std::min(buffSize, stdStr.size() + 1));
  if (ImGui::InputText((std::string("##") + std::to_string((size_t)m_pObj)).c_str(), (char*)&str, buffSize) && str[0] != '\0') {
    type->SetValueFromString(m_pObj, str);
  }
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_Struct* type) {
  void* pObj = m_pObj;
  if (type->parentTypeDesc) {
    type->parentTypeDesc->Accept(this);
  }
  for (const TypeDescriptor_Struct::Member& member : type->members) {
    if (ImGui::TreeNode(member.name)) {
      m_pObj = member.getAddress(pObj);
      member.type->Accept(this);
      ImGui::TreePop();
    }
  }
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_StdVector* type) {
  size_t size = type->getSize(m_pObj);
  const void* pObj = m_pObj;
  for (int i = 0; i < size; i++) {
    m_pObj = type->getItem(pObj, i);
    if (ImGui::TreeNode(std::to_string((size_t)m_pObj).c_str(), type->itemType->GetDynamic(m_pObj)->getFullName().c_str())) {
      type->itemType->Accept(this);
      ImGui::TreePop();
    }
  }
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_Weak_Ptr* type) {
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
  void** ppObj = (void**)m_pObj;
  const TypeDescriptor* concreteType = type->getDynamicType(*ppObj);
  m_pObj = *ppObj;
  concreteType->Accept(this);
}
