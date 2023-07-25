
#include "imgui/imgui.h"

#include "GUI\SceneGUIWindow.h"
#include "GUI\ImGuiTypeVisitor.h"
#include "GUI\GizmoUtils.h"
#include "Core/Engine.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Core/Reflection/CopyTypeVisitor.h"
#include "Core/PrefabManager.h"
#include "Entities/Entity.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"

Node* selectedNode;

#define GUI_ORANGE IM_COL32(255, 69, 0, 255)
#define GUI_WHITE IM_COL32(255, 255, 255, 255)

#define UNIQUE_LABEL(LABEL, PTR) (std::string(LABEL) + "##" + std::to_string((size_t)PTR)).c_str()

std::string entityNamesStr;
std::vector<std::string> entityNames;

void SavePrefabPopup(void* pObj, const reflection::TypeDescriptor* type) {
  if (ImGui::IsItemClicked(1)) {
    ImGui::OpenPopup(UNIQUE_LABEL("CreatePrefab", pObj));
  }
  if (ImGui::BeginPopup(UNIQUE_LABEL("CreatePrefab", pObj))) {
    static constexpr size_t buffSize = 256;
    static char str[buffSize];
    ImGui::Text("Create Prefab");
    ImGui::InputText("Path:", str, buffSize);
    ImGui::SameLine();
    if (ImGui::Button("Save##PrefabFromScene")) {
      void* pCopy = type->create();
      reflection::CopyTypeVisitor visitor(pCopy, pObj);
      type->Accept(&visitor);
      PrefabManager::GetInstance()->SavePrefab(str, pCopy, type);
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void SetupSceneGUIWindow() {
  entityNames = Scene::GetEntityNames();
  for (std::string& name : entityNames) {
    entityNamesStr += name;
    entityNamesStr += '\0';
  }
}

void DrawAddEntityButton() {
  static Node* toAddNode = nullptr;
  if (ImGui::Button((std::string("+##NewEntity") + std::to_string((size_t)selectedNode)).c_str())) {
    toAddNode = selectedNode;
  }

  if (toAddNode == selectedNode) {

    ImGui::Begin("Add Entity");

    static int currItem = -1;
    static void* pObj = nullptr;
    static const reflection::TypeDescriptor_Struct* typeDesc;

    if (ImGui::Combo("##EntityCombo", &currItem, entityNamesStr.c_str())) {
      if(pObj) free(pObj);
      if (currItem >= 0 && currItem < entityNames.size()) {
        typeDesc = static_cast<const reflection::TypeDescriptor_Struct*>(reflection::ReflectionHelper::GetTypeDesc(entityNames[currItem]));
        pObj = malloc(typeDesc->size);
        typeDesc->construct(pObj);
      }
    }

    if (pObj && typeDesc && currItem >= 0 && currItem < entityNames.size()) {
      ImGuiTypeVisitor visitor_imgui(pObj, typeDesc->getFullName());
      typeDesc->Accept(&visitor_imgui);

      if (ImGui::Button("Accept##AcceptEntity") && pObj != nullptr) {
        Entity* entity = Scene::AddEntity(selectedNode, entityNames[currItem].c_str());
        reflection::CopyTypeVisitor visitor_copy(entity, pObj);
        typeDesc->Accept(&visitor_copy);
        typeDesc->setup(entity);
        free(pObj);
        pObj = nullptr;
        toAddNode = nullptr;
        currItem = -1;
      }

      if (ImGui::Button("Cancel##CancelEntity")) {
        free(pObj);
        pObj = nullptr;
        toAddNode = nullptr;
        currItem = -1;
      }
    }    

    ImGui::End();
  }
}

void DrawAddNodeButton(Node* parent) {
  if (ImGui::Button((std::string("+##NewNode") + std::to_string((size_t)parent)).c_str())) {
    Engine::GetActiveScene()->AddNode(nullptr, Transform(), parent);
  }
}

void DrawNodeRecursive(Node* node) {
  if (node == selectedNode) {
    ImGui::PushStyleColor(ImGuiCol_Text, GUI_ORANGE);
  }
  else {
    ImGui::PushStyleColor(ImGuiCol_Text, GUI_WHITE);
  }

  bool open = ImGui::TreeNode((node->m_name + std::string("##Node") + std::to_string((size_t)node)).c_str());

  SavePrefabPopup(node, reflection::TypeResolver<Node>::get());

  ImGui::PopStyleColor();

  if (ImGui::IsItemClicked()) {
    if (node == selectedNode) {
      selectedNode = nullptr;
    }
    else{
    selectedNode = node;
    }
  }

  ImGui::SameLine();
  ImGui::Checkbox((std::string("##CB") + std::to_string((size_t)node)).c_str(), &node->m_enabled);
  if (open) {
    for (Node* child : node->m_children) {
      DrawNodeRecursive(child);
    }
    DrawAddNodeButton(node);
    ImGui::TreePop();
  }
}

void DrawNodeInfo() {
  if (!selectedNode) return;  

  static constexpr size_t buffSize = 64ull;
  static char buff[buffSize];
  memcpy(buff, selectedNode->m_name.c_str(), selectedNode->m_name.size() + 1);  
  ImGui::InputText("##NodeName", buff, buffSize);  
  selectedNode->m_name = buff;

  for (Entity* entity : selectedNode->m_entities) {
    ImGui::Separator();
    //ImGui::PushStyleColor(ImGuiCol_Text, GUI_ORANGE);    
    SavePrefabPopup(entity, reflection::TypeResolver<Entity>::getDynamic(entity));
    //ImGui::PopStyleColor();
    ImGuiTypeVisitor visitor(entity, entity->GetReflectionDynamic()->getFullName());
    entity->GetReflectionDynamic()->Accept(&visitor);
  }
  ImGui::Separator();
  DrawImGuiTransform(selectedNode->m_localTransform, selectedNode->m_globalTransform);
  ImGui::Separator();
  DrawAddEntityButton();
}

void DrawSceneGUIWindow() {
  ImGui::Begin("Scene Window");
  Node* rootNode = Engine::GetActiveScene()->GetRootNode();
  for (Node* child : rootNode->m_children) {
    DrawNodeRecursive(child);
  }
  DrawAddNodeButton(rootNode);
  ImGui::End();

  ImGui::Begin("Inspector");
  DrawNodeInfo();
  ImGui::End();
}