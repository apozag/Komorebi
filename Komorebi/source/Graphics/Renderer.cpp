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

  void HDRITest() {
    static CubeRenderTarget* cubeRT = nullptr; 
    
    static Texture2D* hdri = nullptr;

    static const Drawable* unitCube = Engine::GetRenderer()->GetCubePrimitive();

    static PixelShader* pShader = nullptr;
    static GeometryShader* gShader = nullptr;
    static VertexShader* vShader = nullptr;
    static GeometryConstantBuffer<ViewsCBufferData>* viewsCBuffer = nullptr;
    static InputLayout* inputLayout = nullptr;

    memory::Factory::PushGlobalMode(true);

    if (cubeRT == nullptr) {
      cubeRT = memory::Factory::Create<CubeRenderTarget>(512, 512, DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
      cubeRT->Setup();
    }
    if (hdri == nullptr) {
      hdri = memory::Factory::Create<Texture2D>("assets/images/hdri/pine_attic_4k.hdr", 0u); 
      hdri->Setup();
    }
    if (vShader == nullptr) {
      vShader = memory::Factory::Create<VertexShader>("assets/shaders/hdrEnvMapVertex.cso");
      vShader->Setup();
    }
    if (gShader == nullptr) {
      gShader = memory::Factory::Create<GeometryShader>("assets/shaders/hdrEnvMapGeom.cso");
      gShader->Setup();
    }
    if (pShader == nullptr) {
      pShader = memory::Factory::Create<PixelShader>("assets/shaders/hdrEnvMapPixel.cso");
      pShader->Setup();
    }
    if (viewsCBuffer == nullptr) {
      ViewsCBufferData data;
      DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(_PI2, 1, 0.1, 10);
      data.viewProj[0] = proj;
      data.viewProj[1] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(_PI2), proj);
      data.viewProj[2] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(_PI), proj);
      data.viewProj[3] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(-_PI2), proj);
      data.viewProj[4] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(_PI2), proj);
      data.viewProj[5] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(-_PI2), proj);

      viewsCBuffer = memory::Factory::Create<GeometryConstantBuffer<ViewsCBufferData>>(0, false, data);
    }
    if (inputLayout == nullptr) {
      const D3D11_INPUT_ELEMENT_DESC ied[] =
      {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
      };
      inputLayout = memory::Factory::Create<InputLayout>(ied, 1, *vShader);
    }
    memory::Factory::PopGlobalMode();

    cubeRT->Bind();
    hdri->Bind();
    pShader->Bind();
    gShader->Bind();
    vShader->Bind();
    viewsCBuffer->Bind();
    inputLayout->Bind();

    unitCube->Draw(DirectX::XMMatrixIdentity());

    inputLayout->Unbind();
    viewsCBuffer->Unbind();
    vShader->Unbind();
    gShader->Unbind();
    pShader->Unbind();
    hdri->Unbind();
    cubeRT->Unbind();
  }

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

const Drawable* gfx::Renderer::GetCubePrimitive() const {
  static Drawable* cube = nullptr;
  if (cube == nullptr) {
    cube = (Drawable*)(ModelLoader::GetInstance()->GenerateCube());
  }
  return cube;
}
