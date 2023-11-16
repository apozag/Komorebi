#pragma once

#include "Entities/Entity.h"
#include "Entities/Camera.h"
#include "Graphics/Bindables/Resource/RenderTarget.h"
#include "Graphics/Bindables/Resource/ConstantBuffer.h"
#include "Graphics/PipelineStage.h"
#include "Core/Math/MathWrappers.h"
#include "Graphics/BindableSlotsInfo.h"
#include "Core\Defines.h"

class Transform;

struct alignas(16) LightTransformData {
  DirectX::XMMATRIX m_viewProj;
};

class DirectionalLight : public Entity {
private:
  struct alignas(16) DirLightData {
    POD::Vector4 m_color;
    POD::Vector4 m_dir;
  };
public:
  DirectionalLight() : 
    m_pcbuffer(gfx::ConstantBuffer<DirLightData>(PCBUFF_LIGHTINFO_SLOT, true, NULL, gfx::PipelineStage::PIXEL)),
    m_vcbuffer(gfx::ConstantBuffer<LightTransformData>(PCBUFF_LIGHTTRANSFORM_SLOT, true, NULL, gfx::PipelineStage::PIXEL)) {
  };
  DirectionalLight(DirectX::SimpleMath::Vector3 m_color) :
    m_color(m_color),
    m_pcbuffer(gfx::ConstantBuffer<DirLightData>(PCBUFF_LIGHTINFO_SLOT, true, NULL, gfx::PipelineStage::PIXEL)),
    m_vcbuffer(gfx::ConstantBuffer<LightTransformData>(PCBUFF_LIGHTTRANSFORM_SLOT, true, NULL, gfx::PipelineStage::PIXEL)) {
  };

  ~DirectionalLight();

  virtual void Setup() override;

  void Insert(Node* node, const Transform& worldTransform) override;

  void Bind() const;
  void Unbind() const;

  DirectX::SimpleMath::Vector3 GetColor() const { return m_color; }
  const Camera* GetCamera() const { return m_camera; }
  gfx::RenderTarget* GetShadowMap() const { return m_rt; }
  REFLECT()
private:
  math::Vector3 m_color;
  Camera* m_camera;
  gfx::RenderTarget* m_rt;

  gfx::ConstantBuffer<DirLightData> m_pcbuffer;
  gfx::ConstantBuffer<LightTransformData> m_vcbuffer;
};

class PointLight : public Entity {
private:
  struct alignas(16) PointLightData {
    POD::Vector4 m_color;
    POD::Vector4 pos;
  };

public:
  PointLight() :
    m_pcbuffer(gfx::ConstantBuffer<PointLightData>(PCBUFF_LIGHTINFO_SLOT, true, NULL, gfx::PipelineStage::PIXEL)),
    m_vcbuffer(gfx::ConstantBuffer<LightTransformData>(PCBUFF_LIGHTTRANSFORM_SLOT, true, NULL, gfx::PipelineStage::PIXEL)) {
  };
  PointLight(DirectX::SimpleMath::Vector3 m_color) :
    m_color(m_color),
    m_pcbuffer(gfx::ConstantBuffer<PointLightData>(PCBUFF_LIGHTINFO_SLOT, true, NULL, gfx::PipelineStage::PIXEL)),
    m_vcbuffer(gfx::ConstantBuffer<LightTransformData>(PCBUFF_LIGHTTRANSFORM_SLOT, true, NULL, gfx::PipelineStage::PIXEL)) {
  }

  ~PointLight();

  virtual void Setup() override;

  void Insert(Node* node, const Transform& worldTransform) override;

  void Bind() const;
  void Unbind() const;

  DirectX::SimpleMath::Vector3 GetColor() const { return m_color; }
  const Camera* GetCameras() const { return m_cameras; }
  gfx::RenderTarget* GetShadowMaps() { return m_rts; }
  REFLECT()
private:
  math::Vector3 m_color;
  Camera m_cameras[6];
  gfx::RenderTarget m_rts[6];

  gfx::ConstantBuffer<PointLightData> m_pcbuffer;
  gfx::ConstantBuffer<LightTransformData> m_vcbuffer;
};

class SpotLight : public Entity {
private:
  struct alignas(16) SpotLightData {
    POD::Vector4 m_color;
    POD::Vector4 m_dir;
    POD::Vector4 m_pos;
    POD::Vector4 m_phiTheta;
  };

public:
  SpotLight() :
    m_pcbuffer(gfx::ConstantBuffer<SpotLightData>(PCBUFF_LIGHTINFO_SLOT, true, NULL, gfx::PipelineStage::PIXEL)),
    m_vcbuffer(gfx::ConstantBuffer<LightTransformData>(PCBUFF_LIGHTTRANSFORM_SLOT, true, NULL, gfx::PipelineStage::PIXEL)) {
  };
  SpotLight(DirectX::SimpleMath::Vector3 m_color, float m_phi, float m_theta) :
    m_color(m_color),
    m_phi(m_phi),
    m_theta(m_theta),
    m_pcbuffer(gfx::ConstantBuffer<SpotLightData>(PCBUFF_LIGHTINFO_SLOT, true, NULL, gfx::PipelineStage::PIXEL)),
    m_vcbuffer(gfx::ConstantBuffer<LightTransformData>(PCBUFF_LIGHTTRANSFORM_SLOT, true, NULL, gfx::PipelineStage::PIXEL))
  {}

  ~SpotLight();

  virtual void Setup() override;

  void Insert(Node* node, const Transform& worldTransform) override;

  void Bind() const;
  void Unbind() const;

  DirectX::SimpleMath::Vector3 GetColor() const { return m_color; }
  const Camera* GetCamera() const { return m_camera; }
  gfx::RenderTarget* GetShadowMap() const { return m_rt; }
  REFLECT()
private:
  math::Vector3 m_color;
  float m_phi, m_theta;
  Camera* m_camera;
  gfx::RenderTarget* m_rt;

  gfx::ConstantBuffer<SpotLightData> m_pcbuffer;
  gfx::ConstantBuffer<LightTransformData> m_vcbuffer;
};
