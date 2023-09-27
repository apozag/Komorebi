#include  "Entities/ParticleSource.h"

#include "Core/Reflection/ReflectionImplMacros.h"
#include "Core/Memory/Factory.h"
#include "Graphics/BindableSlotsInfo.h"
#include "Graphics\Bindables\Resource\VertexBuffer.h"
#include "Scene/Scene.h"
#include "Graphics\Graphics.h"

ParticleSource::~ParticleSource() {
  free(m_data);
}

void ParticleSource::Setup() {
  Drawable::Setup();
  size_t size = m_maxParticles * sizeof(POD::Particle);
  m_data = (POD::Particle*)malloc(size);
  //ZeroMemory(m_data, size);
  int columns = 10;
  for (float i = 0; i < m_maxParticles / columns; i++) {
    for (float j = 0; j < columns; j++) {
      m_data[(int)(i * columns + j)].pos = { i * 5.f, j * 5.f, 0.f };
    }
  }
  m_bvhData.m_min = math::Vector3{ 0.f,0.f,0.f };
  m_bvhData.m_max = math::Vector3{ 5.f * m_maxParticles / columns, 5.f * columns, 0.f };
  AddBindable(memory::Factory::Create<gfx::VertexBuffer>(m_data, m_maxParticles * sizeof(POD::Particle), sizeof(POD::Particle), 0, true));

  m_topology = gfx::Topology::POINTS;
}

REFLECT_STRUCT_BEGIN(ParticleSource, Drawable)
REFLECT_STRUCT_MEMBER(m_maxParticles)
REFLECT_STRUCT_END(ParticleSource)

REGISTER_ENTITY(ParticleSource)