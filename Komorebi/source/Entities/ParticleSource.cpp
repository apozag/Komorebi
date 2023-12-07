#include  "Entities/ParticleSource.h"

#include "Core/Engine.h"
#include "Core/Reflection/ReflectionImplMacros.h"
#include "Core/Reflection/UnloadTypeVisitor.h"
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
  
  CopyFromMaterialInstance(&m_preset->m_matInstance);

  size_t size = m_preset->m_maxParticles * sizeof(POD::Particle);
  m_data = (POD::Particle*)malloc(size);
  ZeroMemory(m_data, size);
  m_vBuffer = memory::Factory::Create<gfx::VertexBuffer>(m_data, m_preset->m_maxParticles * sizeof(POD::Particle), sizeof(POD::Particle), 0, true);
  AddBindable(m_vBuffer);
  m_topology = gfx::Topology::POINTS;
  m_currentParticles = 0u;
  m_currentIdx = 0u;
}

void ParticleSource::Insert(Node* node, const Transform& worldTransform) {
  static float cumDeltaTime = 0.f;

  if (m_preset->m_emitterShape == nullptr) {
    return;
  }

  float dt = Engine::GetDeltaTime();  
  
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
    m_data[i].pos += m_data[i].velocity * m_preset->m_speed * dt;
  }

  if ((m_preset->m_loop || m_start) && m_currentTime >= m_preset->m_duration) {
    m_currentTime = 0.f;
    m_start = false;
  }
  
  //m_preset->m_dispersion = fmaxf(fminf(m_dispersion, 1.f), 0.f);

  if (m_currentTime < m_preset->m_duration) {
    cumDeltaTime += dt;
    m_currentTime += dt;
    unsigned int numToSpawn = (unsigned int)(cumDeltaTime * m_preset->m_emissionRate);
    cumDeltaTime = numToSpawn > 0u ? 0.f : cumDeltaTime;
    math::Vector3 forward = worldTransform.GetForward();
    for (unsigned int i = 0u; i < numToSpawn && m_currentParticles < m_preset->m_maxParticles; i++) {
      math::Vector3 point = m_preset->m_emitterShape->SampleRandomPoint();      
      math::Vector3 center = m_preset->m_emitterShape->GetCenter();
      if (m_preset->m_world) {
        point = worldTransform.PointToWorld(point);
        center = worldTransform.PointToWorld(center);
      }
      else {
        point = worldTransform.DirectionToWorld(point);
      }
      math::Vector3 dirToCenter = point - center;
      dirToCenter.Normalize();
      math::Vector3 velocity = dirToCenter * m_preset->m_dispersion + forward * (1.f- m_preset->m_dispersion);
      velocity.Normalize();                     
      m_data[m_currentParticles].pos = { point.x, point.y, point.z };
      m_data[m_currentParticles].velocity = { velocity.x, velocity.y, velocity.z };
      m_data[m_currentParticles].lifeTime = m_preset->m_lifetime;

      m_currentParticles = min(m_currentParticles + 1, m_preset->m_maxParticles);
    }
  }

  SetVertexCount(m_currentParticles);
  m_vBuffer->SetData(m_data, m_currentParticles * sizeof(POD::Particle));  

  Drawable::Insert(node, m_preset->m_world ? Transform::GetIdentity() : worldTransform);
}


REFLECT_STRUCT_BEGIN(ParticleSource, Drawable)
REFLECT_STRUCT_MEMBER(m_start)
REFLECT_STRUCT_MEMBER(m_preset)
REFLECT_STRUCT_END(ParticleSource)

REGISTER_ENTITY(ParticleSource)