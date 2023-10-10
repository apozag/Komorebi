#include "Core/Math/Shapes.h"
#include "Core/Reflection/ReflectionImplMacros.h"

#define _PI 3.14159f

bool PointShape::IsPointInside(const math::Vector3& point) {  
  return (point - m_pos).LengthSquared() < 0.001f;
}

math::Vector3 PointShape::SampleRandomPoint() {

  constexpr float offset = 0.1f;

  float r0 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

  r0 = r0 * 2 - 1;
  r1 = r1 * 2 - 1;
  r2 = r2 * 2 - 1;

  math::Vector3 dir( r0, r1, r2 );
  dir.Normalize();

  return m_pos * dir * offset;;
}

math::Vector3 PointShape::GetCenter() {
  return m_pos;
}

bool CircleShape::IsPointInside(const math::Vector3& point) {
  return (m_center - point).LengthSquared() <= m_radius;
}

math::Vector3 CircleShape::SampleRandomPoint() {
  float r0 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float radius = m_radius * sqrt(r0);
  float theta = r1 * 2 * _PI - _PI;
  return m_center + math::Vector3{ cosf(theta) * radius, sinf(theta) * radius, 0.f};
}

math::Vector3 CircleShape::GetCenter() {
  return m_center;
}

bool SphereShape::IsPointInside(const math::Vector3& point) {
  return (m_center - point).LengthSquared() <= m_radius;
}

math::Vector3 SphereShape::SampleRandomPoint() {
  float r0 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float rho = m_radius * sqrt(r0);
  float theta = r1 * 2 * _PI;
  float phi = r2 * _PI;
  return m_center + math::Vector3{ cosf(theta) * sinf(phi) * rho, sinf(theta) * sinf(phi) * rho, cosf(phi) * rho };
}

math::Vector3 SphereShape::GetCenter() {
  return m_center;
}

bool SquareShape::IsPointInside(const math::Vector3& point) {
  return false;
}

math::Vector3 SquareShape::SampleRandomPoint() {
  float r0 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  return m_center + math::Vector3{m_width * r0 - m_width*0.5f, m_height * r1 - m_height*0.5f, 0.f};
}

math::Vector3 SquareShape::GetCenter() {
  return m_center;
}

REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(Shape)
REFLECT_STRUCT_END(Shape)

REFLECT_STRUCT_BEGIN(PointShape, Shape)
REFLECT_STRUCT_MEMBER(m_pos)
REFLECT_STRUCT_END(PointShape)

REFLECT_STRUCT_BEGIN(CircleShape, Shape)
REFLECT_STRUCT_MEMBER(m_center)
REFLECT_STRUCT_MEMBER(m_radius)
REFLECT_STRUCT_END(CircleShape)

REFLECT_STRUCT_BEGIN(SphereShape, Shape)
REFLECT_STRUCT_MEMBER(m_center)
REFLECT_STRUCT_MEMBER(m_radius)
REFLECT_STRUCT_END(SphereShape)

REFLECT_STRUCT_BEGIN(SquareShape, Shape)
REFLECT_STRUCT_MEMBER(m_center)
REFLECT_STRUCT_MEMBER(m_width)
REFLECT_STRUCT_MEMBER(m_height)
REFLECT_STRUCT_END(SquareShape)

IMPLEMENT_REFLECTION_POINTER(Shape)
