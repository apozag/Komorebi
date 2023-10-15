#include "GUI/ImGuiEditPtrTypeVisitor.h"

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
#define UNIQUE_LABEL2(LABEL) (std::string(LABEL) + std::to_string((size_t)ppObj)).c_str()

using namespace reflection;

void ImGuiEditPtrTypeVisitor::Visit(const TypeDescriptor_Weak_Ptr* type) {
}

void ImGuiEditPtrTypeVisitor::Visit(const TypeDescriptor_Owned_Ptr* type) {

  static void** currEditPPtr = nullptr;
  static std::vector<const reflection::TypeDescriptor_Struct*> possibleTypes;
  static std::string possibleTypesStr;
  static const reflection::TypeDescriptor* dynamicTypeDesc = nullptr;

  void** ppObj = (void**)m_pObj;
  m_pObj = *ppObj;

  // pointer not set yet
  if (*ppObj == nullptr) {

    if (!currEditPPtr && ImGui::Button(UNIQUE_LABEL2("Create"))) {
      currEditPPtr = ppObj;

      // fetch Types that inherit from itemType
      const TypeDescriptor_Struct* itemTypeStruct = dynamic_cast<const reflection::TypeDescriptor_Struct*>(type);
      if (itemTypeStruct) {
        possibleTypes = ReflectionHelper::GetChildTypes(itemTypeStruct);
        possibleTypesStr = "";
        for (const reflection::TypeDescriptor_Struct* childType : possibleTypes) {
          possibleTypesStr += childType->name;
          possibleTypesStr += '\0';
        }
      }
    }    
  }
  
  if (currEditPPtr != nullptr) {
    // Edit window for new element
    if (currEditPPtr == ppObj) {
      ImGui::Begin("Initial data");

      // Combo with child types. When chosen, free and allocate new temporal object
      static int currItem = 0;
      if (possibleTypes.size() > 1 && ImGui::Combo(UNIQUE_LABEL2("##EntityCombo"), &currItem, possibleTypesStr.c_str())) {
        dynamicTypeDesc = possibleTypes[currItem];
        //if (*currEditPPtr != nullptr) dynamicTypeDesc->destroy(*currEditPPtr);
        if (currItem >= 0 && currItem < possibleTypes.size()) {
          *currEditPPtr = dynamicTypeDesc->create();
        }
      }

      // Show (and edit) object info
      dynamicTypeDesc->Accept(this);

      // If accept, setup object
      if (ImGui::Button(UNIQUE_LABEL2("Accept"))) {
        reflection::SetupTypeVisitor setupVisitor(*currEditPPtr);
        dynamicTypeDesc->Accept(&setupVisitor);
        dynamicTypeDesc->destroy(*currEditPPtr);
        currEditPPtr = nullptr;
      }

      m_pObj = *ppObj;
    }
    else {
      dynamicTypeDesc->Accept(this);
    }
  }
}

void ImGuiEditPtrTypeVisitor::Visit(const reflection::TypeDescriptor_Asset_Ptr* type) {

  if (ImGui::TreeNode(UNIQUE_LABEL(m_currName + "##"))) {    
    static constexpr size_t buffSize = 256;
    static char str[buffSize];    

    void** ppObj = type->getPPtr(m_pObj);

    if (*ppObj == nullptr) {
      ImGui::InputText("Asset filename", str, buffSize);
      if (ImGui::Button(UNIQUE_LABEL("Accept"))) {
        const TypeDescriptor* dynamicType = type->GetDynamic(ppObj);
        *ppObj = PrefabManager::GetInstance()->LoadPrefab(str, dynamicType, false);
        type->setFilename(m_pObj, str);
      }
    }
    else {      
      const std::string& filename = type->getFilename(m_pObj);
      if (const PrefabManager::PrefabInfo* prefabInfo = PrefabManager::GetInstance()->GetPrefabInfo(filename.c_str())) {
        static const PrefabManager::PrefabInfo* selectedPrefab = nullptr;
        bool open = selectedPrefab == prefabInfo || ImGui::Button("Open Prefab");
        if (open) {          
          selectedPrefab = prefabInfo;
          ImGui::Begin((prefabInfo->m_name + "##" + filename).c_str());
          ImGuiEditPtrTypeVisitor visitor(prefabInfo->m_ptr, prefabInfo->m_typeDesc->GetDynamic(prefabInfo->m_ptr)->getFullName());
          prefabInfo->m_typeDesc->Accept(&visitor);
          if (ImGui::Button((std::string("Close##Prefab") + filename).c_str())) {
            selectedPrefab = nullptr;
          }
          ImGui::End();
        }
      }
    }
    ImGui::TreePop();
  }
}

