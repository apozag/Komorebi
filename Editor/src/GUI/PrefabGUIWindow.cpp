#include "GUI/PrefabGUIWindow.h"

#include "imgui/imgui.h"

#include "Core/PrefabManager.h"
#include "GUI/ImGuiTypeVisitor.h"

#define GUI_ORANGE IM_COL32(255, 69, 0, 255)
#define GUI_WHITE IM_COL32(255, 255, 255, 255)

void SetupPrefabGUIWindow() {

}

void DrawPrefabGUIWindow() {
  ImGui::Begin("Prefabs");

  std::vector<PrefabInfo>& prefabs = PrefabManager::GetInstance()->GetLoadedPrefabs();
  for (PrefabInfo& prefab : prefabs) {
    ImGui::PushStyleColor(ImGuiCol_Text, GUI_ORANGE);    
    bool open = ImGui::TreeNode((prefab.m_name + "##" + prefab.m_fileName).c_str());
    ImGui::PopStyleColor();
    if (open) {
      ImGuiTypeVisitor visitor(prefab.m_ptr);
      prefab.m_typeDesc->Accept(&visitor);
      ImGui::TreePop();
    }
  }

  ImGui::End();
}
