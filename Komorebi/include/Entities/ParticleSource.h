#pragma once

#include "Core/Defines.h"
#include "Entities/Drawable.h"
#include "Core/Reflection/TypeDescriptors.h"
#include "FX/ParticleSourcePreset.h"

class Shape;

class ParticleSource : public Drawable {
public:
  ParticleSource() {}
  ~ParticleSource();

  virtual void Setup() override;
  void Insert(Node* node, const Transform& worldTransform) override;

  REFLECT()

private:
  /*unsigned int m_maxParticles = 0u;    
  bool m_loop = false;
  bool m_world = false;
  float m_dispersion = 0.f;
  float m_duration = 0.f;
  float m_emissionRate = 0.f;
  float m_speed = 0.f;
  float m_lifetime = 0.f;
  OWNED_PTR(Shape) m_emitterShape;*/

  bool m_start = false;
  ASSET_PTR(ParticleSourcePreset) m_preset;
  
  POD::Particle* m_data;
  gfx::VertexBuffer* m_vBuffer;
  unsigned int m_currentParticles;
  unsigned int m_currentIdx;
  float m_currentTime = 0.f;
};