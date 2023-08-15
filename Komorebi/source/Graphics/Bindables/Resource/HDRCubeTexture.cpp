#include "Graphics/Bindables/Resource/HDRCubeTexture.h"
#include "Core/Reflection/ReflectionImplMacros.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/State/PixelShader.h"
#include "Graphics/Bindables/State/VertexShader.h"
#include "Graphics/Bindables/State/GeometryShader.h"
#include "Graphics/Bindables/State/CubeRenderTarget.h"
#include "Graphics/Bindables/State/InputLayout.h"
#include "Graphics/Bindables/State/RasterizerState.h"
#include "Graphics/Bindables/State/Viewport.h"
#include "Graphics/Bindables/Resource/ConstantBuffer.h"
#include "Graphics/Renderer.h"
#include "Core/Engine.h"
#include "Entities/Drawable.h"
#include "Core\Memory\Factory.h"

#define _PI 3.141592f
#define _PI2 1.570796f

namespace gfx {

  struct ViewsCBufferData {
    DirectX::XMMATRIX viewProj[6];
  };

  HDRCubeTexture::~HDRCubeTexture() {    
  }

  void HDRCubeTexture::Setup() {

    CubeRenderTarget* tempCubeRT = memory::Factory::Create<CubeRenderTarget>(m_size, m_size, DXGI_FORMAT_R32G32B32A32_FLOAT, m_slot);
    tempCubeRT->Setup();

    Texture2D* hdri = memory::Factory::Create<Texture2D>(m_filename, 0u);
    hdri->Setup();

    static const Drawable* unitCube = Engine::GetRenderer()->GetCubePrimitive();

    static PixelShader* pShader = nullptr;
    static GeometryShader* gShader = nullptr;
    static VertexShader* vShader = nullptr;

    static PixelShader* pConvShader = nullptr;

    static GeometryConstantBuffer<ViewsCBufferData>* viewsCBuffer = nullptr;
    static InputLayout* inputLayout = nullptr;
    static SamplerState* samplerState = nullptr;
    static RasterizerState* rasterState = nullptr;
    static Viewport* viewport = nullptr;

    memory::Factory::PushGlobalMode(true);
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
    if (pConvShader == nullptr) {
      pConvShader = memory::Factory::Create<PixelShader>("assets/shaders/CubemapConvolutionPixel.cso");
      pConvShader->Setup();
    }
    if (viewsCBuffer == nullptr) {      
      ViewsCBufferData data;
      DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(_PI2, 1, 0.1, 10);

      data.viewProj[0] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationY(_PI2)), proj));
      data.viewProj[1] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationY(-_PI2)), proj));
      data.viewProj[2] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(-_PI2)), proj));
      data.viewProj[3] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(_PI2)), proj));       
      data.viewProj[4] = DirectX::XMMatrixTranspose(proj);
      data.viewProj[5] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationY(_PI)), proj));     

      viewsCBuffer = memory::Factory::Create<GeometryConstantBuffer<ViewsCBufferData>>(0, false, data);      
    }
    if (inputLayout == nullptr) {
      const D3D11_INPUT_ELEMENT_DESC ied[] =
      {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
      };      
      inputLayout = memory::Factory::Create<InputLayout>(ied, 4, *vShader);
    }
    if (samplerState == nullptr) {
      samplerState = memory::Factory::Create<SamplerState>(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, 0);
      samplerState->Setup();
    }
    if (rasterState == nullptr) {
      rasterState = memory::Factory::Create<RasterizerState>(true);
      rasterState->Setup();
    }
    if (viewport == nullptr) {
      viewport = memory::Factory::Create<Viewport>(0, 0, m_size, m_size);
      viewport->Setup();
    }
    memory::Factory::PopGlobalMode();

    // Bind general stuff
    inputLayout->Bind();
    samplerState->Bind();
    rasterState->Bind();
    viewport->Bind();
    gShader->Bind();
    vShader->Bind();
    viewsCBuffer->Bind();

    // 2D Texture to cubemap
    tempCubeRT->Bind();
    hdri->Bind();
    pShader->Bind();  

    unitCube->Draw(DirectX::XMMatrixIdentity());
        
    pShader->Unbind();
    hdri->Unbind();
    tempCubeRT->Unbind();

    // Cubemap convolution
    m_cubeRT = memory::Factory::Create<CubeRenderTarget>(m_size, m_size, DXGI_FORMAT_R32G32B32A32_FLOAT, m_slot);
    m_cubeRT->Setup();

    m_cubeRT->Bind();
    tempCubeRT->GetCubeTexture()->Bind();
    pConvShader->Bind();

    unitCube->Draw(DirectX::XMMatrixIdentity());

    pConvShader->Unbind();
    tempCubeRT->GetCubeTexture()->Unbind();
    m_cubeRT->Unbind();

    // Unbind general stuff
    viewport->Unbind();
    rasterState->Unbind();
    samplerState->Unbind();
    inputLayout->Unbind();
    viewsCBuffer->Unbind();
    vShader->Unbind();
    gShader->Unbind();

    m_srv = m_cubeRT->GetCubeTexture()->GetSRV();

    memory::Factory::Destroy(tempCubeRT);
    
  }

  REFLECT_STRUCT_BEGIN(HDRCubeTexture, CubeTexture)
  REFLECT_STRUCT_MEMBER(m_size)
  REFLECT_STRUCT_END(HDRCubeTexture)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, HDRCubeTexture)

