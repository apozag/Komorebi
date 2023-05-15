#include "GUI/MaterialGUIWindow.h"

#include "imgui/imgui.h"

#include "Core/PrefabManager.h"
#include "Graphics/Material.h"
#include "Graphics/Bindables/Resource/ReflectedConstantBuffer.h"

#define GUI_ORANGE IM_COL32(255, 69, 0, 255)
#define GUI_WHITE IM_COL32(255, 255, 255, 255)

void SetupMaterialGUIWindow() {

}

void DrawMaterialInfo(const PrefabManager::PrefabInfo& prefabInfo) {
  //ImGui::Text((std::string("Path: ") + prefabInfo.m_fileName).c_str());
  gfx::Material* material = (gfx::Material*)prefabInfo.m_ptr;
  for (gfx::ReflectedConstantBuffer* cbuffer : material->GetReflectedConstantBuffers()) {
    for (const gfx::ReflectedConstantBuffer::ConstantBufferVariable& variable : cbuffer->GetVariables()) {
      if (variable.typeDesc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT) {
        switch (variable.typeDesc.Class) {
        case D3D_SHADER_VARIABLE_CLASS::D3D10_SVC_SCALAR:
        {
          float value = cbuffer->GetFloat(variable.desc.Name);
          if (ImGui::InputFloat(variable.desc.Name, &value)) {
            cbuffer->SetFloat(variable.desc.Name, value);
          }
        }
          break;
        case D3D_SHADER_VARIABLE_CLASS::D3D_SVC_VECTOR:
        {
          float values[4];
          cbuffer->GetVector4(variable.desc.Name, values);
          if (ImGui::InputFloat4(variable.desc.Name, values)) {
            cbuffer->SetVector4(variable.desc.Name, values);
          }
        }
        break;
        default:
          break;
        }
      }
    }    
    ImGui::Separator();
  }
}

void DrawMaterialGUIWindow() {
  ImGui::Begin("Materials");
  auto materialPrefabs = PrefabManager::GetInstance()->GetLoadedPrefabs<gfx::Material>();
  for (const PrefabManager::PrefabInfo& prefabInfo : materialPrefabs) {
    ImGui::PushStyleColor(ImGuiCol_Text, GUI_ORANGE);
    bool open = ImGui::TreeNode(prefabInfo.m_name.c_str());
    ImGui::PopStyleColor();
    if (open) {
      DrawMaterialInfo(prefabInfo);
      ImGui::TreePop();
    }
  }
  ImGui::End();
}