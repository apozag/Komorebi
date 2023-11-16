#pragma once

#include <vector>
#include <string>

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

class Camera;
class Transform;

namespace gfx {

class RenderStep;
class GeometryRenderStep;
class RenderTarget;
class Texture2D;
struct CameraView;
struct Job;

class RenderPipeline : public GameObject{
  friend class Renderer;
public:

  RenderPipeline() {}
  RenderPipeline(const char* name, std::vector<OWNED_PTR(RenderStep)> steps) : m_name(name), m_steps(steps) {}

  void Setup() override;
  void Execute(const CameraView& camView, std::vector<Job>& jobs) const;

  const std::string& GetName() const { return m_name; }

  const RenderStep* GetRenderStep(unsigned int idx) const;
  const GeometryRenderStep* FindGeomRenderStep(const Job& job) const;

  REFLECT_BASE()

private:  

  std::string m_name;
  std::vector<OWNED_PTR(RenderStep)> m_steps;

};

}
