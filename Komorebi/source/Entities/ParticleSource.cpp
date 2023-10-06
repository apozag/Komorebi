#include  "Entities/ParticleSource.h"

#include <cstdlib>

#include "Core/Engine.h"
#include "Core/Reflection/ReflectionImplMacros.h"
#include "Core/Memory/Factory.h"
#include "Core/Math/Shapes.h"
#include "Graphics/BindableSlotsInfo.h"
#include "Graphics/Bindables/Resource/VertexBuffer.h"
#include "Scene/Scene.h"
#include "Graphics/Graphics.h"

ParticleSource::~ParticleSource() {
  free(m_data);
}

void ParticleSource::Setup() {
  Drawable::Setup();
  size_t size = m_maxParticles * sizeof(POD::Particle);
  m_data = (POD::Particle*)malloc(size);
  ZeroMemory(m_data, size);
  m_vBuffer = memory::Factory::Create<gfx::VertexBuffer>(m_data, m_maxParticles * sizeof(POD::Particle), sizeof(POD::Particle), 0, true);
  AddBindable(m_vBuffer);
  m_topology = gfx::Topology::POINTS;
  m_currentParticles = 0u;
  m_currentIdx = 0u;
}

void ParticleSource::Insert(Node* node, const Transform& worldTransform) {
  static float cumDeltaTime = 0.f;

  if (m_emitterShape == nullptr) {
    return;
  }

  float dt = Engine::GetDeltaTime();

  cumDeltaTime += dt;

  unsigned int numToSpawn = (unsigned int) (cumDeltaTime * m_emissionRate);

  cumDeltaTime = numToSpawn > 0u ? 0.f : cumDeltaTime;
  
  {
    for (unsigned i = 0u; i < m_currentParticles; i++) {
      m_data[i].lifeTime -= dt;
      if (m_data[i].lifeTime <= 0.f) {
        m_currentParticles--;
        if (m_currentParticles >= 0) {
          m_data[i] = m_data[m_currentParticles];
          m_data[i].lifeTime -= dt;
          m_data[m_currentParticles] = {};
        }
      }
      m_data[i].pos += m_data[i].velocity * m_speed * dt;      
    }
  }

  for(unsigned int i = 0u; i < numToSpawn && m_currentParticles < m_maxParticles; i++) {
    float r0 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    math::Vector3 point = m_emitterShape->SampleRandomPoint({ r0, r1 });
    math::Vector3 center = m_emitterShape->GetCenter();
    math::Vector3 velocity = point - center;
    velocity.Normalize();
    m_data[m_currentParticles].pos = { point.x, point.y, point.z };
    m_data[m_currentParticles].velocity = { velocity.x, velocity.y, velocity.z};
    m_data[m_currentParticles].lifeTime = m_lifetime;
    
    m_currentParticles = min(m_currentParticles + 1, m_maxParticles);
  }

  SetVertexCount(m_currentParticles);
  m_vBuffer->SetData(m_data, m_currentParticles * sizeof(POD::Particle));

  Drawable::Insert(node, worldTransform);
}


REFLECT_STRUCT_BEGIN(ParticleSource, Drawable)
REFLECT_STRUCT_MEMBER(m_maxParticles)
REFLECT_STRUCT_MEMBER(m_emitterShape)
REFLECT_STRUCT_MEMBER(m_emissionRate)
REFLECT_STRUCT_MEMBER(m_speed)
REFLECT_STRUCT_MEMBER(m_lifetime)
REFLECT_STRUCT_END(ParticleSource)

REGISTER_ENTITY(ParticleSource)