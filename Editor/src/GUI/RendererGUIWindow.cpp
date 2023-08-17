#include "GUI/RendererGUIWindow.h"

#include <map>

#include "imgui/imgui.h"

#include "Core/Engine.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderInfo.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Graphics\Bindables\State\BlendState.h"
#include "Graphics\Bindables\State\PixelShader.h"
#include "Graphics\Bindables\State\VertexShader.h"
#include "Graphics/Material.h"
#include "Entities/Light.h"
#include "Entities/Drawable.h"
#include "Core/Memory/Factory.h"
#include "Core/PrefabManager.h"
#include "GUI/ImGuiTypeVisitor.h"

static std::map<const gfx::Texture2D*, gfx::RenderTarget*> rtMap;


void DrawImageNoAlpha(const gfx::Texture2D* tex2D) {  
  
  auto entry = rtMap.find(tex2D);
  if (entry != rtMap.end()) {
    ImGui::Image((void*)entry->second->GetTextures2D()[0]->GetSRV(), ImVec2(300, 300));
  }
}

void Blit(const gfx::Texture2D* src, const gfx::RenderTarget* dst) {
  const static gfx::Material* blitMat;
  if (blitMat == nullptr) {
    blitMat = PrefabManager::GetInstance()->LoadPrefab<gfx::Material>("assets/materials/blitNoAlphaMat.xml", true);
  }

  const static Drawable* quad = Engine::GetRenderer()->GetQuadPrimitive();

  dst->Bind();
  src->BindAt(0);  
  blitMat->Bind();
  blitMat->GetPasses()[0]->Bind();
  quad->Draw(DirectX::XMMatrixIdentity());
  blitMat->GetPasses()[0]->Unbind();
  blitMat->Unbind();
  src->UnbindAt(0);
  dst->Unbind();
}

void UpdateRts(float /*timeStep*/) {  
  for (const auto& pair : Engine::GetRenderer()->GetRenderInfo()->GetGlobalRenderTargets()) {
    for (const gfx::Texture2D* tex : pair.m_rt.GetTextures2D()) {
      auto entry = rtMap.find(tex);
      if (entry != rtMap.end()) {
        entry->second->Clear(0.f, 0.f, 0.f);
        Blit(entry->first, entry->second);
      }
    }
  }
}


void SetupRendererGUIWindow() {

  constexpr int texSize = 256;

  for (const auto& pair : Engine::GetRenderer()->GetRenderInfo()->GetGlobalRenderTargets()) {    
    for (const gfx::Texture2D* tex : pair.m_rt.GetTextures2D()) {
      auto entry = rtMap.find(tex);
      if (entry == rtMap.end()) {        
        gfx::RenderTarget* rt = memory::Factory::Create<gfx::RenderTarget>(texSize, texSize, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0);
        rt->Setup();
        rtMap.emplace(tex, rt);
      }
    }
  }

  const reflection::TypeDescriptor* resourceType = reflection::TypeResolver<gfx::ResourceBindable>::get();
  const reflection::TypeDescriptor* textureType = reflection::TypeResolver<gfx::Texture2D>::get();
  for (const auto& pair : Engine::GetRenderer()->GetRenderInfo()->GetGlobalResources()) {
    const reflection::TypeDescriptor* resType = pair.m_resource->GetReflectionDynamic();
    if (resType != nullptr && resType == textureType) {
      gfx::Texture2D* tex = (gfx::Texture2D*)pair.m_resource.m_ptr;       
      gfx::RenderTarget * rt = memory::Factory::Create<gfx::RenderTarget>(texSize, texSize, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0);
      rt->Setup();
      rt->Clear(0.f, 0.f, 0.f);
      Blit(tex, rt);
      rtMap.emplace(tex, rt);      
    }
  }

  Engine::AddPostRenderCallback(UpdateRts);
}

void DrawRendererGUIWindow() {
  ImGui::Begin("Renderer");
  if (ImGui::TreeNode("RenderInfo")) {
    gfx::RenderInfo* info = Engine::GetRenderer()->GetRenderInfo();
    ImGuiTypeVisitor visitor(info, "RenderInfo");
    reflection::TypeResolver<gfx::RenderInfo>::get()->Accept(&visitor);
    ImGui::TreePop();
  } 
  if (ImGui::TreeNode("RenderTargets")) {
    for (const auto& pair : Engine::GetRenderer()->GetRenderInfo()->GetGlobalRenderTargets()) {
      if (ImGui::TreeNode(pair.m_name.c_str())) {
        for (const gfx::Texture2D* tex : pair.m_rt.GetTextures2D()) {
          //ImGui::Image((void*)tex->GetSRV(), ImVec2(300, 300));
          DrawImageNoAlpha(tex);
        }
        ImGui::TreePop();
      }
    }    
    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Resources")) {
    static const reflection::TypeDescriptor* resourceType = reflection::TypeResolver<gfx::ResourceBindable>::get();
    static const reflection::TypeDescriptor* textureType = reflection::TypeResolver<gfx::Texture2D>::get();
    for (const auto& pair : Engine::GetRenderer()->GetRenderInfo()->GetGlobalResources()) {
      const reflection::TypeDescriptor* resType = pair.m_resource->GetReflectionDynamic();
      if (resType != nullptr) {
        if (ImGui::TreeNode(pair.m_name.c_str())) {
          if (resType == textureType) {
            gfx::Texture2D* tex = (gfx::Texture2D*)pair.m_resource.m_ptr;            
            //ImGui::Image((void*)tex->GetSRV(), ImVec2(300, 300));            
            DrawImageNoAlpha(tex);
          }
          ImGui::TreePop();
        }
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
