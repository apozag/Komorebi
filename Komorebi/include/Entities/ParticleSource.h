#pragma once

#include "Core/Defines.h"
#include "Entities/Drawable.h"

class ParticleSource : public Drawable {
public:
  ParticleSource() {}
  ~ParticleSource();

  virtual void Setup() override;

  REFLECT()

private:
  unsigned int m_maxParticles;
  POD::Particle* m_data;
};