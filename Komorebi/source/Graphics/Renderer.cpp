#define NOMINMAX

#include <algorithm>
#include <iostream>
#include <sstream>
#include <ctime>

#include "Core/Engine.h"
#include "Core/Math/Transform.h"
#include "Core/Memory/Factory.h"
#include "Core/Reflection/DeserializationTypeVisitor.h"
#include "Entities/Drawable.h"
#include "Graphics/Pass.h"
#include "Graphics/Renderer.h"
#include "Entities/Light.h"
#include "Entities/Camera.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/Resource/Texture3D.h"
#include "Graphics/Material.h"
#include "Graphics\RenderPipeline.h"
#include "Graphics\RenderInfo.h"

#include "Scene\ModelLoader.h"

namespace gfx {
  bool compareCamera(const gfx::CameraView& c1, const gfx::CameraView& c2) {
    return (c1.camera->m_priority < c2.camera->m_priority);
  }
}

gfx::Renderer::Renderer() :
  m_dirLightsCbuff(PixelConstantBuffer<DirLightData>(PCBUFF_DIRLIGHT_SLOT)),
  m_pointLightsCbuff(PixelConstantBuffer<PointLightData>(PCBUFF_POINTLIGHT_SLOT)),
  m_spotLightsCbuff(PixelConstantBuffer<SpotLightData>(PCBUFF_SPOTLIGHT_SLOT)),
  m_lightTransformCbuff(VertexConstantBuffer<LightTransformData>(VCBUFF_LIGHTTRANSFORM_SLOT)),
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

  m_PCFFiltersSampler.Bind();

  m_shadowInfoCbuff.SetBuffer(ShadowInfoData{
    1024.f,				// Shadowmap size
    1.0f / 1024,			// 1/Shadowmap Size
    (float)windowSize,
    (float)filterSize,
    1.0f / filterSize
    });
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


  /// DEBUG
  /*
  m_renderInfo = new RenderInfo();
  m_renderInfo->m_globalRTs.push_back(
    {
      "TEST",
      RenderTarget(1024, 1024, DXGI_FORMAT_R8G8B8A8_UINT, 1, 8)
    });
  m_renderInfo->m_renderPipelines.push_back(
    RenderPipeline{
      "FW_STEP",
      {
        RenderPipeline::RenderStep{
          std::vector<std::string>(),
          std::vector<std::string>(),
          "DEFAULT",
          0,
          false,
          false
        }
      }
    });

  rapidxml::xml_document<> doc;
  reflection::SerializationTypeVisitor visitor(m_renderInfo, &doc);
  m_renderInfo->GetReflection().Accept(&visitor);
  reflection::ReflectionHelper::ResolvePendingPointers();
  std::ofstream myfile;
  myfile.open("renderInfo.xml");
  myfile << doc;
  myfile.close();
  reflection::ReflectionHelper::ClearTrackedStrings();
  */
}

void gfx::Renderer::Init() {

  const reflection::TypeDescriptor* renderInfoType = reflection::DefaultResolver::get<RenderInfo>();

  reflection::SetupTypeVisitor setupVisitor(m_renderInfo);
  renderInfoType->Accept(&setupVisitor);

  reflection::ReflectionHelper::ClearAll();

  m_dirLightData.count = 0;
  m_spotLightData.count = 0;
  m_pointLightData.count = 0;

}

void gfx::Renderer::SubmitDrawable(const Drawable* drawable, const Transform* transform, Material* material) {
  for (Pass* pass : material->GetPasses()) {
    //Opaque:			29 empty + 1 culling + 2 layer + 8 matIdx + 8 passIdx + 16 depth
    //Transparent:		29 empty + 1 culling + 2 layer + 16 depth + 8 matIdx + 8 passIdx

    bool isTransparent = pass->m_layer == PASSLAYER_TRANSPARENT;

    // Pass layer
    uint64_t key = uint64_t(pass->m_layer) << 32;

    // Material (resource binds)
    unsigned int idx = material->GetIdx();
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
  if (m_spotLightData.count >= MAX_DIRLIGHTS) return;

  const DirectX::SimpleMath::Vector3& c = spotlight->GetColor();
  const DirectX::SimpleMath::Vector3& f = worldTransform->GetForward();
  const DirectX::SimpleMath::Vector3& p = worldTransform->GetPositionUnsafe();

  m_spotLightData.m_color[m_spotLightData.count] = { c.x, c.y, c.z };
  m_spotLightData.dir[m_spotLightData.count] = { f.x, f.y, f.z };
  m_spotLightData.pos[m_spotLightData.count] = { p.x, p.y, p.z, };
  m_spotLightData.count++;
}

void gfx::Renderer::SubmitDirectionalLight(const DirectionalLight* dirlight, const Transform* worldTransform) {
  if (m_dirLightData.count >= MAX_DIRLIGHTS) return;

  const DirectX::SimpleMath::Vector3& c = dirlight->GetColor();
  const DirectX::SimpleMath::Vector3& f = -worldTransform->GetForward();

  m_dirLightData.m_color[m_dirLightData.count] = { c.x, c.y, c.z };
  m_dirLightData.dir[m_dirLightData.count] = { f.x, f.y, f.z };

  m_lightTransformData.viewProj[m_dirLightData.count] = DirectX::XMMatrixTranspose(
    DirectX::XMMatrixMultiply(
      worldTransform->GetInverseMatrixUnsafe(),
      dirlight->GetCamera()->getProj()
    )
  );

  // Shadow mapping camera
  m_shadowCameras.push_back({ dirlight->GetCamera() , worldTransform });
  m_shadowMaps.push_back(dirlight->GetShadowMap());

  m_dirLightData.count++;
}

void gfx::Renderer::SubmitPointLight(const PointLight* pointlight, const Transform* worldTransform) {
  if (m_pointLightData.count >= MAX_DIRLIGHTS) return;

  const DirectX::SimpleMath::Vector3& c = pointlight->GetColor();
  const DirectX::SimpleMath::Vector3& f = worldTransform->GetForward();

  m_pointLightData.m_color[m_pointLightData.count] = { c.x, c.y, c.z };
  m_pointLightData.pos[m_pointLightData.count] = { f.x, f.y, f.z };
  m_pointLightData.count++;
}

void gfx::Renderer::SubmitCamera(const Camera* camera, const Transform* worldTransform) {
  m_cameras.push_back({ camera, worldTransform });
}

void gfx::Renderer::Render() {

  // Light info to pixel shader
  m_dirLightsCbuff.SetBuffer(m_dirLightData);
  m_pointLightsCbuff.SetBuffer(m_pointLightData);
  m_spotLightsCbuff.SetBuffer(m_spotLightData);

  m_dirLightsCbuff.Update();
  m_pointLightsCbuff.Update();
  m_spotLightsCbuff.Update();

  m_dirLightsCbuff.Bind();
  m_pointLightsCbuff.Bind();
  m_spotLightsCbuff.Bind();

  // Light transforms to vertex shader
  m_lightTransformCbuff.SetBuffer(m_lightTransformData);
  m_lightTransformCbuff.Update();
  m_lightTransformCbuff.Bind();

  // Render shadowmaps
  static RenderPipeline* shadowRenderPipeline;
  if (shadowRenderPipeline == nullptr) {
    shadowRenderPipeline = memory::Factory::Create<RenderPipeline>();
    shadowRenderPipeline->m_steps.push_back(RenderStep());
  }
  for (int i = 0; i < m_shadowCameras.size(); i++) {
    shadowRenderPipeline->m_steps[0].m_outRt = m_shadowMaps[i];
    shadowRenderPipeline->Execute(m_shadowCameras[i], m_jobs);
  }

  for (RenderTarget* rt : m_shadowMaps) {
    rt->GetTextures2D()[0]->Bind();
  }

  // Cameras sorted by priority.
  std::sort(m_cameras.begin(), m_cameras.end(), compareCamera);

  Engine::GetDefaultRendertarget()->Clear(0.f, 0.f, 0.f);

  // Draw scene
  for (int i = 0; i < m_cameras.size(); i++) {
    CameraView camView = m_cameras[i];
    const RenderPipeline* pipeline = m_renderInfo->FindRenderPipeline(camView.camera->GetRenderPipelineId());
    if (pipeline != nullptr) {
      pipeline->Execute(camView, m_jobs);
    }
  }

  for (RenderTarget* rt : m_shadowMaps) {
    rt->GetTextures2D()[0]->Unbind();
  }

  m_jobs.clear();
  m_cameras.clear();
  m_shadowCameras.clear();
  m_shadowMaps.clear();
  m_dirLightData.count = 0;
  m_pointLightData.count = 0;
  m_spotLightData.count = 0;
}

const Drawable* gfx::Renderer::GetQuadPrimitive() const {
  static Drawable* quad = nullptr;
  if (quad == nullptr) {
    quad = (Drawable*)(ModelLoader::GetInstance()->GenerateQuad());
  }
  return quad;
}

