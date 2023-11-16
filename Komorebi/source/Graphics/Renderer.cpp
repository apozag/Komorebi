#define NOMINMAX

#include <algorithm>
#include <iostream>
#include <sstream>
#include <ctime>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Engine.h"
#include "Core/Math/Transform.h"
#include "Core/Memory/Factory.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Core/Reflection/ReflectionHelper.h"
#include "Core/PrefabManager.h"
#include "Entities/Drawable.h"
#include "Graphics/Pass.h"
#include "Graphics/Renderer.h"
#include "Entities/Light.h"
#include "Entities/Camera.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/Resource/Texture3D.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderPipeline/ClearRenderStep.h"
#include "Graphics/RenderPipeline/GeometryRenderStep.h"
#include "Graphics/RenderInfo.h"
#include "Graphics/PipelineStage.h"

#include "Scene\ModelLoader.h"

// DEBUG
#include "Graphics/Bindables/State/GeometryShader.h"
#include "Graphics/Bindables/State/PixelShader.h"
#include "Graphics/Bindables/State/CubeRenderTarget.h"
#include "Graphics/Bindables/State/InputLayout.h"

#define _PI 3.141592f
#define _PI2 1.570796f

// DEBUG

namespace gfx {

  struct ViewsCBufferData {
    DirectX::XMMATRIX viewProj[6];
  };

  bool compareCamera(const gfx::CameraView& c1, const gfx::CameraView& c2) {
    return (c1.camera->m_priority < c2.camera->m_priority);
  }
}

gfx::Renderer::Renderer() :
  m_shadowInfoCbuff(ConstantBuffer<ShadowInfoData>(PCBUFF_SHADOW_SLOT, true, nullptr, PipelineStage::PIXEL)),
  m_globalCbuff(ConstantBuffer<GlobalData>(PCBUFF_GLOBAL_SLOT, true, nullptr, PipelineStage::ALL)),
  m_shadowMapSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, SRV_SHADOWMAP_SLOT),
  m_PCFFiltersSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, SRV_PCF_SLOT) {
  // Initialize PCF filter random values
  int windowSize = 32;
  int filterSize = 5;
  int bufferSize = windowSize * windowSize * filterSize * filterSize * 2 * sizeof(float);
  float* randBuffer = (float*)malloc(bufferSize);
  std::srand(std::time(nullptr));
  int idx = 0;
  for (int i = 0; i < windowSize * windowSize; i++) {
    for (int u = 0; u < filterSize; u++) {
      for (int v = 0; v < filterSize; v++) {
        randBuffer[idx++] = float(u) - int(filterSize * 0.5) + ((float)std::rand() / RAND_MAX);
        randBuffer[idx++] = float(v) - int(filterSize * 0.5) + ((float)std::rand() / RAND_MAX);
      }
    }
  }

  m_PCFFilters = memory::Factory::Create<gfx::Texture3D>((unsigned char*)randBuffer,
    filterSize * filterSize,
    windowSize,
    windowSize,
    DXGI_FORMAT_R32G32_FLOAT, 8, SRV_PCF_SLOT);
  m_PCFFilters->Bind();

  m_PCFFiltersSampler.Setup();
  m_PCFFiltersSampler.Bind();

  m_shadowInfoCbuff.m_buffer = ShadowInfoData {
    1024.f,				    // Shadowmap size
    1.0f / 1024,			// 1/Shadowmap Size
    (float)windowSize,
    (float)filterSize,
    1.0f / filterSize
  };
  m_shadowInfoCbuff.Update();
  m_shadowInfoCbuff.Bind();

  m_renderTargets.push_back(Engine::GetDefaultRendertarget());

  rapidxml::file<> xmlFile("renderInfo.xml");
  rapidxml::xml_document<> doc;
  doc.parse<0>(xmlFile.data());

  const reflection::TypeDescriptor* renderInfoType = reflection::DefaultResolver::get<RenderInfo>();

  m_renderInfo = memory::Factory::Create<RenderInfo>();

  reflection::DeserializationTypeVisitor deserializationVisitor(m_renderInfo, doc.first_node());
  renderInfoType->Accept(&deserializationVisitor);

  reflection::ReflectionHelper::ResolvePendingPointers();

  reflection::ReflectionHelper::ClearAll();

  m_shadowRenderPipeline = memory::Factory::Create<RenderPipeline>();
  m_shadowRenderPipeline->m_steps.push_back(memory::Factory::Create<ClearRenderStep>(std::vector<RenderStep::TextureInfo>{}, ""));
  m_shadowRenderPipeline->m_steps.push_back(memory::Factory::Create<GeometryRenderStep>(std::vector<RenderStep::TextureInfo>{}, "", 99, false));  
}

void gfx::Renderer::Init() {

  {
    reflection::SetupTypeVisitor setupVisitor(m_renderInfo);
    reflection::DefaultResolver::get<RenderInfo>()->Accept(&setupVisitor);
  }

  {
    reflection::SetupTypeVisitor setupVisitor(m_shadowRenderPipeline);
    reflection::TypeResolver<RenderPipeline>::get()->Accept(&setupVisitor);
  }

  reflection::ReflectionHelper::ClearAll();

  GetQuadPrimitive();

}

void gfx::Renderer::SubmitDrawable(const Drawable* drawable, const Transform* transform, MaterialInstance* material) {
  for (const Pass* pass : material->GetMaterial()->GetPasses()) {   
    m_jobs.push_back(Job{
      0u,
      drawable,
      transform,
      pass,
      material
      });
  }
}

void gfx::Renderer::SubmitSpotlight(SpotLight* spotlight, const Transform* worldTransform) {
  // Shadow mapping camera
  m_lightViews.push_back({ spotlight->GetCamera() , worldTransform, spotlight->GetShadowMap() });
  m_spotLights.push_back(spotlight);
}

void gfx::Renderer::SubmitDirectionalLight(DirectionalLight* dirlight, const Transform* worldTransform) {  
  m_lightViews.push_back({ dirlight->GetCamera() , worldTransform, dirlight->GetShadowMap()});
  m_dirLights.push_back(dirlight);
}

void gfx::Renderer::SubmitPointLight(PointLight* pointlight, const Transform* worldTransform) {
  for (int i = 0; i < 6; i++) {
    m_lightViews.push_back({ pointlight->GetCameras() + i, worldTransform, pointlight->GetShadowMaps() + i });
  }
  m_pointLights.push_back(pointlight);
}

void gfx::Renderer::SubmitCamera(const Camera* camera, const Transform* worldTransform) {
  m_camViews.push_back({ camera, worldTransform });
}

void gfx::Renderer::Render() {   

  // Render shadowmaps  
  for (LightView& lv : m_lightViews) {
    m_shadowRenderPipeline->m_steps[0]->m_outRt = lv.m_rt;
    m_shadowRenderPipeline->m_steps[1]->m_outRt = lv.m_rt;
    m_shadowRenderPipeline->Execute({ lv.m_camera, lv.m_transform }, m_jobs);
  }

  // Cameras sorted by priority.
  std::sort(m_camViews.begin(), m_camViews.end(), compareCamera);

  m_globalCbuff.m_buffer.time = Engine::GetGameTime();
  m_globalCbuff.Update();
  m_globalCbuff.Bind();

  // Draw scene
  for (int i = 0; i < m_camViews.size(); i++) {
    CameraView camView = m_camViews[i];
    const RenderPipeline* pipeline = m_renderInfo->FindRenderPipeline(camView.camera->m_renderPipelineId);
    if (pipeline != nullptr) {
      pipeline->Execute(camView, m_jobs);
    }
  }
}

void gfx::Renderer::Clear() {
  m_jobs.clear();
  m_camViews.clear();
  m_lightViews.clear();
  m_dirLights.clear();
  m_spotLights.clear();
  m_pointLights.clear();
}

void gfx::Renderer::Blit(const gfx::Texture2D* src, const gfx::RenderTarget* dst, const gfx::Material* material) {
  
  if (material == nullptr) {
    static const gfx::Material* defaultBlitMat = nullptr;
    if (defaultBlitMat == nullptr) {
      defaultBlitMat = PrefabManager::GetInstance()->LoadPrefab<gfx::Material>("assets/materials/blitMat.xml", true);
    }
    material = defaultBlitMat;
  }

  const static Drawable* quad = GetQuadPrimitive();

  dst->Bind();
  src->BindAt(0u);
  material->Bind();
  for (Pass* pass : material->GetPasses()) {
    pass->Bind();
    quad->Draw(DirectX::XMMatrixIdentity());
    pass->Unbind();
  }
  material->Unbind();
  src->UnbindAt(0u);
  dst->Unbind();
}

const Drawable* gfx::Renderer::GetQuadPrimitive() {
  static Drawable* quad = nullptr;
  if (quad == nullptr) {
    quad = (Drawable*)(ModelLoader::GetInstance()->GenerateQuad());
  }
  return quad;
}

const Drawable* gfx::Renderer::GetCubePrimitive() {
  static Drawable* cube = nullptr;
  if (cube == nullptr) {
    cube = (Drawable*)(ModelLoader::GetInstance()->GenerateCube());
  }
  return cube;
}
