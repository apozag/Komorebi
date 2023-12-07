#include "FX/ParticleSourcePreset.h"
#include "Core/Reflection/ReflectionImplMacros.h"

REFLECT_STRUCT_BASE_BEGIN(ParticleSourcePreset)
REFLECT_STRUCT_MEMBER(m_maxParticles)
REFLECT_STRUCT_MEMBER(m_loop)
REFLECT_STRUCT_MEMBER(m_world)
REFLECT_STRUCT_MEMBER(m_dispersion)
REFLECT_STRUCT_MEMBER(m_duration)
REFLECT_STRUCT_MEMBER(m_emissionRate)
REFLECT_STRUCT_MEMBER(m_speed)
REFLECT_STRUCT_MEMBER(m_lifetime)
REFLECT_STRUCT_MEMBER(m_emitterShape)
REFLECT_STRUCT_MEMBER(m_matInstance)
REFLECT_STRUCT_END(ParticleSourcePreset)

IMPLEMENT_REFLECTION_ASSET_POINTER(ParticleSourcePreset)