
#include "imgui/imgui.h"

#include "GUI\SceneGUIWindow.h"
#include "GUI\ImGuiTypeVisitor.h"
#include "Core/Engine.h"
#include "Entities/Entity.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"

Node* selectedNode;

void DrawNodeRecursive(Node* node) {
  if (node == selectedNode) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
  }
  else {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
  }

  bool open = ImGui::TreeNode((node->m_name + std::string("##Node") + std::to_string((size_t)node)).c_str());

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
    ImGui::Text(entity->GetReflectionDynamic()->name);
    ImGuiTypeVisitor visitor(entity);
    entity->GetReflectionDynamic()->Accept(&visitor);
  }
}

void DrawSceneGUIWindow() {
  ImGui::Begin("Scene Window");
  Node* rootNode = Engine::m_activeScene->GetRootNode();
  for (Node* child : rootNode->m_children) {
    DrawNodeRecursive(child);
  }
  ImGui::End();

  ImGui::Begin("Inspector");
  DrawNodeInfo();
  ImGui::End();
}