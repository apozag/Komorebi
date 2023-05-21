#include "GUI/ImGuiTypeVisitor.h"
#include "GUI/ImGuiTypeVisitor.h"

#include <string>
#include <vector>

#include "imgui/imgui.h"

#include "Core/Memory/Factory.h"
#include "Core/Reflection/CopyTypeVisitor.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Core/PrefabManager.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"

#define UNIQUE_LABEL(LABEL) (std::string(LABEL) + std::to_string((size_t)m_pObj)).c_str()
#define UNIQUE_LABEL2(LABEL) (std::string(LABEL) + std::to_string((size_t)pObj)).c_str()

using namespace reflection;

void ImGuiTypeVisitor::Visit(const TypeDescriptor* type) {
  constexpr const size_t buffSize = 256;
  std::string stdStr = type->GetValueStr(m_pObj);
  char str[buffSize];
  memcpy(str, stdStr.c_str(), stdStr.size()+1);
  if (ImGui::InputText(UNIQUE_LABEL("##"), (char*)&str, buffSize) && str[0] != '\0') {
    type->SetValueFromString(m_pObj, str);
    m_dirty = true;
  }
}

void ImGuiTypeVisitor::Visit(const TypeDescriptor_Struct* type) {  
  void* pObj = m_pObj;
  bool prevDirty = m_dirty;

  // Save as prefab
  if (ImGui::IsItemClicked(1)) {
    ImGui::OpenPopup("CreatePrefab");
  }
  if (ImGui::BeginPopup("CreatePrefab")) {
    static constexpr size_t buffSize = 216;
    static char str[buffSize];
    ImGui::Text("Create Prefab");
    ImGui::InputText("Path:", str, buffSize);
    if (ImGui::Button("Save##PrefabFromScene")) {
      void* pCopy = type->create();
      CopyTypeVisitor visitor(pCopy, pObj);
      type->Accept(&visitor);
      PrefabManager::GetInstance()->SavePrefab(str, pCopy, type);
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  if (type->parentTypeDesc) {
    type->parentTypeDesc->Accept(this);
  }

  if (strcmp(type->name, "Node") == 0) {
    Node* pNode = (Node*)pObj;
    ImGui::SameLine();
    ImGui::Checkbox(UNIQUE_LABEL("##CB"), &pNode->m_enabled);
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

  const reflection::TypeDescriptor_Ptr* ptrTypeDesc = dynamic_cast<const reflection::TypeDescriptor_Ptr*>(type->itemType);
  const bool isPointerType = ptrTypeDesc != nullptr;

  for (int i = 0; i < size; i++) {
    m_pObj = type->getItem(pObj, i);
    if (ImGui::TreeNode(UNIQUE_LABEL(""), type->itemType->GetDynamic(m_pObj)->getFullName().c_str())) {

      // Delete button
      ImGui::SameLine();
      if (ImGui::Button(UNIQUE_LABEL("-##DeleteElem"))) {
        type->remove(pObj, i);
        memory::Factory::Destroy(isPointerType? *(void**) m_pObj : m_pObj);
        // The vector was modified! stop iterating for this frame
        ImGui::TreePop();
        return;
      }

      m_dirty = false;
      type->itemType->Accept(this);
      ImGui::TreePop();
    }
  }

  // Add new element
  static void* pCurrVector = nullptr;
  static void* pTempElem = nullptr;
  static const reflection::TypeDescriptor* typeDesc = nullptr;
  static bool currVectorIsPointerType = false;

  static std::vector<const reflection::TypeDescriptor_Struct*> possibleTypes;
  static std::string possibleTypesStr;

  if (!pCurrVector && ImGui::Button(UNIQUE_LABEL2("+##AddElem"))) {

    pCurrVector = pObj;
    currVectorIsPointerType = isPointerType;
    
    // For pointers (even if struct), get static type
    if (currVectorIsPointerType) {
      typeDesc = ptrTypeDesc->getStaticType();
    }
    // Any other just get the type
    else {
      typeDesc = type->itemType;
    }    

    // If its a struct, fetch Types that inherit from itemType
    const TypeDescriptor_Struct* itemTypeStruct = dynamic_cast<const reflection::TypeDescriptor_Struct*>(typeDesc);
    if (itemTypeStruct) {
      possibleTypes = ReflectionHelper::GetChildTypes(itemTypeStruct);
      possibleTypesStr = "";
      for(const reflection::TypeDescriptor_Struct* childType : possibleTypes){
        possibleTypesStr += childType->name;
        possibleTypesStr += '\0';
      }
    }

    // Allocate our temporal object (base class if struct pointer)
    if (pTempElem) free(pTempElem);
    pTempElem = malloc(typeDesc->size);
    typeDesc->construct(pTempElem);
  }

  // Edit window for new element
  if (pCurrVector == pObj) {
    ImGui::Begin("Add Element");

    // Combo with child types. When chosen, free and allocate new temporal object
    static int currItem = 0;
    if (possibleTypes.size() > 1 && ImGui::Combo(UNIQUE_LABEL2("##EntityCombo"), &currItem, possibleTypesStr.c_str())) {
      if (pTempElem) free(pTempElem);
      if (currItem >= 0 && currItem < possibleTypes.size()) {
        typeDesc = possibleTypes[currItem];
        pTempElem = malloc(typeDesc->size);
        typeDesc->construct(pTempElem);
      }
    }

    // Show (and edit) object info
    ImGuiTypeVisitor visitor_imgui(pTempElem);
    typeDesc->Accept(&visitor_imgui);

    // If accept, copy to real element and push 
    if (ImGui::Button("Accept##AcceptButton")) {
      const TypeDescriptor_Struct* itemTypeStruct = dynamic_cast<const reflection::TypeDescriptor_Struct*>(typeDesc);      
      if (currVectorIsPointerType) {
        void* pElem = typeDesc->create();
        reflection::CopyTypeVisitor copyVisitor(pElem, pTempElem);
        typeDesc->Accept(&copyVisitor);
        reflection::SetupTypeVisitor setupVisitor(pElem);
        typeDesc->Accept(&setupVisitor);
        type->pushBack(pObj, &pElem);
      }
      else {
        reflection::SetupTypeVisitor setupVisitor(pTempElem);
        typeDesc->Accept(&setupVisitor);
        type->pushBack(pObj, pTempElem);
      }
      pCurrVector = nullptr;
      prevDirty = true;
    }

    // Cancel to close window
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
  if (m_pObj != nullptr) {
    concreteType->Accept(this);
  }
  else {
    ImGui::Text("NULL");
  }
}

void ImGuiTypeVisitor::Visit(const reflection::TypeDescriptor_Asset_Ptr* type) {
  
  const std::string& filename = type->getFilename(m_pObj);
  ImGui::Text(type->getFilename(m_pObj).c_str());
  const PrefabManager::PrefabInfo* prefabInfo = PrefabManager::GetInstance()->GetPrefabInfo(filename.c_str());
  if (!prefabInfo) return;

  static const PrefabManager::PrefabInfo* selectedPrefab = nullptr;
  bool open = selectedPrefab == prefabInfo || ImGui::Button("Open Prefab");
  if (open) {
    selectedPrefab = prefabInfo;
    ImGui::Begin((prefabInfo->m_name + "##" + filename).c_str());
    ImGuiTypeVisitor visitor(prefabInfo->m_ptr);
    prefabInfo->m_typeDesc->Accept(&visitor);
    if (ImGui::Button((std::string("Close##Prefab") + filename).c_str())) {
      selectedPrefab = nullptr;
    }
    ImGui::End();
  }

  /*
  constexpr const size_t buffSize = 256;
  const std::string& filename = type->getFilename(m_pObj);
  char str[buffSize];
  memcpy(str, filename.c_str(), filename.size() + 1);
  if (ImGui::InputText(UNIQUE_LABEL("##AssetPtr"), str, buffSize)) {
    type->setFilename(m_pObj, str);
  }

  if (*ppObj == nullptr) {
    if (ImGui::Button(UNIQUE_LABEL("Load Asset##"))) {
      const TypeDescriptor* dynamicType = type->GetDynamic(m_pObj);
      *ppObj = dynamicType->create();
      PrefabManager::GetInstance()->LoadPrefab(filename.c_str(), *ppObj, type->GetDynamic(m_pObj));
    }
  }
  else {
    if (ImGui::Button(UNIQUE_LABEL("Save Asset##"))) {
      PrefabManager::GetInstance()->SavePrefab(filename.c_str(), *ppObj, type->GetDynamic(m_pObj));
    }
  }*/
}

void ImGuiTypeVisitor::Visit(const reflection::TypeDescriptor_Enum* type) {
  using EnumVal = ::reflection::TypeDescriptor_Enum::EnumValue;
  int currValue = *(int*)m_pObj;
  int currItemIdx = -1;
  std::string names;
  for (int i = 0; i < type->values.size(); i++) {
    const EnumVal& val = type->values[i];
    if (currValue == val.value) {
      currItemIdx = i;
    }
    names += val.name;
    names += '\0';
  }
  if (ImGui::Combo(UNIQUE_LABEL("##EntityCombo"), &currItemIdx, names.c_str())) {
    *(int*)m_pObj = type->values[currItemIdx].value;
    m_dirty = true;
  }
}

void ImGuiTypeVisitor::Visit(const reflection::TypeDescriptor_Bitmask* type) {
  ImGui::Text(type->GetValueStr(m_pObj).c_str());
  ImGui::SameLine();
  static bool open = false;
  if (!open) {
    open |= ImGui::Button((std::string("Edit##EditBitmask") + std::to_string((size_t)m_pObj)).c_str());
  }
  if (open) {
    ImGui::Begin(type->enumType->name);
    int bitmask = *(int*)m_pObj;
    using EnumVal = ::reflection::TypeDescriptor_Enum::EnumValue;
    for (const EnumVal& val : type->enumType->values) {
      bool on = (val.value & bitmask) != 0;
      if (ImGui::Checkbox(val.name, &on)) {
        m_dirty = true;
      }
      if (on) {
        *(int*)m_pObj |= val.value;
      }
      else {
        *(int*)m_pObj &= ~val.value;
      }
    }

    ImGui::Separator();

    if (ImGui::Button("Close##CloseBitmask")) {
      open = false;
    }

    ImGui::End();
  }
}
