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
#include "Graphics/RenderInfo.h"

#include "Scene\ModelLoader.h"

namespace gfx {
  bool compareCamera(const gfx::CameraView& c1, const gfx::CameraView& c2) {
    return (c1.camera->m_priority < c2.camera->m_priority);
  }
}

gfx::Renderer::Renderer() :
  m_shadowInfoCbuff(PixelConstantBuffer<ShadowInfoData>(PCBUFF_SHADOW_SLOT)),
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
  m_shadowRenderPipeline->m_steps.push_back(RenderStep(RenderStep::Type::CLEAR, {}, "", 0, false));
  m_shadowRenderPipeline->m_steps.push_back(RenderStep(RenderStep::Type::DEFAULT, {}, "", 0xFFFFFFFF, false));
}

void gfx::Renderer::Init() {

  const reflection::TypeDescriptor* renderInfoType = reflection::DefaultResolver::get<RenderInfo>();

  reflection::SetupTypeVisitor setupVisitor(m_renderInfo);
  renderInfoType->Accept(&setupVisitor);

  reflection::ReflectionHelper::ClearAll();

  GetQuadPrimitive();

}

void gfx::Renderer::SubmitDrawable(const Drawable* drawable, const Transform* transform, MaterialInstance* material) {
  for (const Pass* pass : material->GetMaterial()->GetPasses()) {
    //Opaque:			    29 empty + 1 culling + 2 layer + 8 matIdx + 8 passIdx + 16 depth
    //Transparent:		29 empty + 1 culling + 2 layer + 16 depth + 8 matIdx + 8 passIdx

    bool isTransparent = pass->m_layer == PASSLAYER_TRANSPARENT;

    // Pass layer
    uint64_t key = uint64_t(pass->m_layer) << 32;

    // Material (resource binds)
    unsigned int idx = material->GetMaterial()->GetIdx();
    unsigned int shifts = 8 + 16 * isTransparent;
    key |= ((uint64_t)idx) << shifts;

    // Pass index (state binds)
    shifts = !isTransparent * 16;
    key |= uint64_t(pass->GetIdx()) << shifts;

    m_jobs.push_back(Job{
      key,
      drawable,
      transform,
      pass,
      material
      });
  }
}

void gfx::Renderer::SubmitSpotlight(const SpotLight* spotlight, const Transform* worldTransform) {
  // Shadow mapping camera
  m_lightViews.push_back({ spotlight->GetCamera() , worldTransform, spotlight->GetShadowMap() });
  m_spotLights.push_back(spotlight);
}

void gfx::Renderer::SubmitDirectionalLight(const DirectionalLight* dirlight, const Transform* worldTransform) {  
  m_lightViews.push_back({ dirlight->GetCamera() , worldTransform, dirlight->GetShadowMap()});
  m_dirLights.push_back(dirlight);
}

void gfx::Renderer::SubmitPointLight(const PointLight* pointlight, const Transform* worldTransform) {
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
  for (const LightView& lv : m_lightViews) {
    m_shadowRenderPipeline->m_steps[0].m_outRt = lv.m_rt;
    m_shadowRenderPipeline->m_steps[1].m_outRt = lv.m_rt;
    m_shadowRenderPipeline->Execute({ lv.m_camera, lv.m_transform }, m_jobs);
  }

  // Cameras sorted by priority.
  std::sort(m_camViews.begin(), m_camViews.end(), compareCamera);

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

const Drawable* gfx::Renderer::GetQuadPrimitive() const {
  static Drawable* quad = nullptr;
  if (quad == nullptr) {
    quad = (Drawable*)(ModelLoader::GetInstance()->GenerateQuad());
  }
  return quad;
}
