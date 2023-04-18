#include "GUI/RendererGUIWindow.h"

#include "imgui/imgui.h"

#include "Core/Engine.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderInfo.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Entities/Light.h"
#include "GUI/ImGuiTypeVisitor.h"

void SetupRendererGUIWindow() {
  
}

void DrawRendererGUIWindow() {
  ImGui::Begin("Renderer");
  if (ImGui::TreeNode("RenderInfo")) {
    gfx::RenderInfo* info = Engine::GetRenderer()->GetRenderInfo();
    ImGuiTypeVisitor visitor(info);
    reflection::TypeResolver<gfx::RenderInfo>::get()->Accept(&visitor);
    ImGui::TreePop();
  } 
  if (ImGui::TreeNode("RenderTargets")) {
    for (const auto& pair : Engine::GetRenderer()->GetRenderInfo()->GetGlobalRenderTargets()) {
      if (ImGui::TreeNode(pair.m_name.c_str())) {
        for (const gfx::Texture2D* tex : pair.m_rt.GetTextures2D()) {
          ImGui::Image((void*)tex->GetSRV(), ImVec2(300, 300));
        }
        ImGui::TreePop();
      }
    }    
    ImGui::TreePop();
  }
  if (ImGui::TreeNode("ShadowMaps")) {
    const std::vector<const DirectionalLight*>& dirlights = Engine::GetRenderer()->GetDirLights();
    for (int i = 0; i < dirlights.size(); i++) {
      if (ImGui::TreeNode((std::string("DirLight") + std::to_string(i)).c_str())) {        
        ImGui::Image((void*)dirlights[i]->GetShadowMap()->GetTextures2D()[0]->GetSRV(), ImVec2(300, 300));
        ImGui::TreePop();
      }
    }    
    ImGui::TreePop();
  }
  ImGui::End();
}
