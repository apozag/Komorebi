#include "GUI/ImGuiTypeVisitor.h"

#include <string>

#include "imgui/imgui.h"

#include "Core/Memory/Allocator.h"
#include "Core/Reflection/CopyTypeVisitor.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"

using namespace reflection;

void ImGuiTypeVisitor::Visit(const TypeDescriptor* type) {
  constexpr const size_t buffSize = 64;
  std::string stdStr = type->GetValueStr(m_pObj);
  char str[buffSize];
  memcpy(str, stdStr.c_str(), stdStr.size()+1);
  if (ImGui::InputText((std::string("##") + std::to_string((size_t)m_pObj)).c_str(), (char*)&str, buffSize) && str[0] != '\0') {
    type->SetValueFromString(m_pObj, str);
    m_dirty = true;
  }
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_Struct* type) {  
  void* pObj = m_pObj;
  bool prevDirty = m_dirty;

  if (type->parentTypeDesc) {
    type->parentTypeDesc->Accept(this);
  }

  if (strcmp(type->name, "Node") == 0) {
    Node* pNode = (Node*)pObj;
    ImGui::SameLine();
    ImGui::Checkbox((std::string("##CB") + std::to_string((size_t)m_pObj)).c_str(), &pNode->m_enabled);
  }

  bool currDirty = false;

  for (const TypeDescriptor_Struct::Member& member : type->members) {
    if (ImGui::TreeNode(member.name)) {
      m_pObj = member.getAddress(pObj);
      m_dirty = false;
      member.type->Accept(this);
      currDirty |= m_dirty;
      ImGui::TreePop();
    }
  }

  if (currDirty) {
    type->reconfigure(pObj);
  }

  m_dirty = prevDirty;


}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_StdVector* type) {
  size_t size = type->getSize(m_pObj);
  void* pObj = m_pObj;
  bool prevDirty = m_dirty;

  for (int i = 0; i < size; i++) {
    m_pObj = type->getItem(pObj, i);
    if (ImGui::TreeNode(std::to_string((size_t)m_pObj).c_str(), type->itemType->GetDynamic(m_pObj)->getFullName().c_str())) {
      m_dirty = false;
      type->itemType->Accept(this);
      ImGui::TreePop();
    }
  }

  // Add new element
  static void* pCurrVector = nullptr;
  static void* pTempElem = nullptr;
  static const reflection::TypeDescriptor* typeDesc = nullptr;
  static bool isPointerType = false;

  if (!pCurrVector && ImGui::Button((std::string("+##AddElem")).c_str())) {
    pCurrVector = pObj;
    if (pTempElem) free(pTempElem);
    const reflection::TypeDescriptor_Ptr* ptrTypeDesc = dynamic_cast<const reflection::TypeDescriptor_Ptr*>(type->itemType);
    if (ptrTypeDesc) {
      typeDesc = ptrTypeDesc->getStaticType();
      isPointerType = true;
    }
    else {
      typeDesc = type->itemType;
      isPointerType = false;
    }    
    pTempElem = malloc(typeDesc->size);
    typeDesc->construct(pTempElem);
  }

  if (pCurrVector == pObj) {
    ImGui::Begin("Add Element");
    ImGuiTypeVisitor visitor_imgui(pTempElem);
    typeDesc->Accept(&visitor_imgui);

    if (ImGui::Button("Accept##AcceptButton")) {
      const TypeDescriptor_Struct* itemTypeStruct = dynamic_cast<const reflection::TypeDescriptor_Struct*>(typeDesc);
      if (isPointerType) {
        void* pElem = typeDesc->create();
        reflection::CopyTypeVisitor visitor_copy(pElem, pTempElem);
        typeDesc->Accept(&visitor_copy);
        if (itemTypeStruct) {
          itemTypeStruct->setup(pElem);
        }
        type->pushBack(pObj, &pElem);
      }
      else {
        type->pushBack(pObj, pTempElem);
      }
      pCurrVector = nullptr;
      prevDirty = true;
    }

    if (ImGui::Button("Cancel##AddEntityCancel")) {
      pCurrVector = nullptr;
    }
    ImGui::End();
  }

  m_dirty = prevDirty;
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_Weak_Ptr* type) {
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {
  void** ppObj = (void**)m_pObj;
  const TypeDescriptor* concreteType = type->getDynamicType(*ppObj);
  m_pObj = *ppObj;
  concreteType->Accept(this);
}
