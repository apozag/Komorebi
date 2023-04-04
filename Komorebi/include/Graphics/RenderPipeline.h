#pragma once

#include <vector>
#include <string>

#include "Core/GameObject.h"
#include "Core\Reflection\ReflectionMacros.h"
#include "Graphics\RenderPipeline\RenderStep.h"

class Camera;
class Transform;

namespace gfx {

class RenderTarget;
class Texture2D;

class RenderPipeline : public GameObject{
  friend class Renderer;
public:

  RenderPipeline() {}
  RenderPipeline(const char* name, std::vector<RenderStep> steps) : m_name(name), m_steps(steps) {}

  void Setup();
  void Execute(CameraView camView, std::vector<Job>& jobs) const;

  const std::string& GetName() const { return m_name; }

  const RenderStep* GetRenderStep(unsigned int idx) const;
  const RenderStep* FindRenderStep(const Job& job) const;

  REFLECT_BASE()

private:  

  std::string m_name;
  std::vector<RenderStep> m_steps;

};

}
