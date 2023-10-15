#pragma once

//#include "Core/Reflection/ReflectionMacros.h"
#include "Core\Math\Shapes.h"
#include "Graphics/MaterialInstance.h"

class ParticleSourcePreset : public GameObject {
public:
  unsigned int m_maxParticles = 0u;  
  bool m_loop = false;
  bool m_world = false;
  float m_dispersion = 0.f;
  float m_duration = 0.f;
  float m_emissionRate = 0.f;
  float m_speed = 0.f;
  float m_lifetime = 0.f;

  OWNED_PTR(Shape) m_emitterShape;
  gfx::MaterialInstance m_matInstance;

  REFLECT_BASE()
};

DECLARE_REFLECTION_ASSET_POINTER(ParticleSourcePreset)