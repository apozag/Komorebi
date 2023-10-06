#pragma once

#include "Core/Math/MathWrappers.h"

class Shape : public GameObject{
public:
  virtual bool IsPointInside(const math::Vector3& point) = 0;
  virtual math::Vector3 SampleRandomPoint(const math::Vector2& uniformSamples) = 0;
  virtual math::Vector3 GetCenter() = 0;
  REFLECT_BASE()
};

class PointShape : public Shape {
public:
  bool IsPointInside(const math::Vector3& point) override;
  math::Vector3 SampleRandomPoint(const math::Vector2& uniformSamples) override;
  math::Vector3 GetCenter() override;
  REFLECT()

  math::Vector3 m_pos;
};

class CircleShape : public Shape {
public:
  bool IsPointInside(const math::Vector3& point) override;
  math::Vector3 SampleRandomPoint(const math::Vector2& uniformSamples) override;
  math::Vector3 GetCenter() override;
  REFLECT()

  math::Vector3 m_center;
  float m_radius;
};

class SquareShape : public Shape {
public:
  bool IsPointInside(const math::Vector3& point) override;
  math::Vector3 SampleRandomPoint(const math::Vector2& uniformSamples) override;
  math::Vector3 GetCenter() override;
  REFLECT()

  math::Vector3 m_center;
  float m_width, m_height;
};

DECLARE_REFLECTION_POINTER(Shape)
