#include "Core/Math/Shapes.h"
#include "Core/Reflection/ReflectionImplMacros.h"

#define _PI 3.14159f

bool PointShape::IsPointInside(const math::Vector3& point) {  
  return (point - m_pos).LengthSquared() < 0.001f;
}

math::Vector3 PointShape::SampleRandomPoint(const math::Vector2& /*uniformSamples*/) {
  return m_pos;
}

math::Vector3 PointShape::GetCenter() {
  return m_pos;
}

bool CircleShape::IsPointInside(const math::Vector3& point) {
  return (m_center - point).LengthSquared() <= m_radius;
}

math::Vector3 CircleShape::SampleRandomPoint(const math::Vector2& uniformSamples) {
  float radius = m_radius * uniformSamples.x * uniformSamples.x;
  float theta = uniformSamples.y * 2 * _PI - _PI;
  return m_center + math::Vector3{ cosf(theta) * radius, sinf(theta) * radius, 0.f};
}

math::Vector3 CircleShape::GetCenter() {
  return m_center;
}

bool SquareShape::IsPointInside(const math::Vector3& point) {
  return false;
}

math::Vector3 SquareShape::SampleRandomPoint(const math::Vector2& uniformSamples) {
  return m_center + math::Vector3{m_width * uniformSamples.x - m_width*0.5f, m_height * uniformSamples.y - m_height*0.5f, 0.f};
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

REFLECT_STRUCT_BEGIN(SquareShape, Shape)
REFLECT_STRUCT_MEMBER(m_center)
REFLECT_STRUCT_MEMBER(m_width)
REFLECT_STRUCT_MEMBER(m_height)
REFLECT_STRUCT_END(SquareShape)

IMPLEMENT_REFLECTION_POINTER(Shape)
