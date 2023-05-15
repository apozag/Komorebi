#include "GUI/PrefabGUIWindow.h"

#include "imgui/imgui.h"

#include "Core/PrefabManager.h"
#include "Core/Engine.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"
#include "GUI/ImGuiTypeVisitor.h"

#define GUI_ORANGE IM_COL32(255, 69, 0, 255)
#define GUI_WHITE IM_COL32(255, 255, 255, 255)

void SetupPrefabGUIWindow() {

}

void DrawPrefabGUIWindow() {
  ImGui::Begin("Prefabs");

  std::vector<PrefabManager::PrefabInfo>& prefabs = PrefabManager::GetInstance()->GetAllLoadedPrefabs();
  for (PrefabManager::PrefabInfo& prefab : prefabs) {
    ImGui::PushStyleColor(ImGuiCol_Text, GUI_ORANGE);    
    bool open = ImGui::TreeNode((prefab.m_name + "##" + prefab.m_fileName).c_str());
    ImGui::PopStyleColor();
    if (open) {
      ImGuiTypeVisitor visitor(prefab.m_ptr);
      prefab.m_typeDesc->Accept(&visitor);
      ImGui::TreePop();
    }
  }

  ImGui::Separator();

  static bool openLoadPrefab = false;
  openLoadPrefab |= ImGui::Button("Load Node Prefab##Button");
  if (openLoadPrefab) {
    ImGui::Begin("Load Node Prefab Into Scene##Window");
    static constexpr size_t buffSize = 64ull;
    static char buff[buffSize];
    ImGui::InputText("Filename##PrefabLoad", buff, buffSize);
    if (ImGui::Button("Load##PrefabLoad")) {      
      Engine::GetActiveScene()->AddPrefabNode(buff, Transform());
      openLoadPrefab = false;
    }
    if (ImGui::Button("Cancel##PrefabLoad")) {
      openLoadPrefab = false;
    }
    ImGui::End();
  }

  ImGui::End();
}
