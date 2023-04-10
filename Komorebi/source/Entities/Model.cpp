#define NOMINMAX

#include <algorithm>

#include "Core/Engine.h"
#include "Scene/Scene.h"
#include "Entities/Mesh.h"
#include "Entities/SkinnedMesh.h"
#include "Entities/Model.h"
#include "Animation/Skeleton.h"
#include "Animation/Animation.h"
#include "Graphics/Material.h"

#include "Scene/ModelLoader.h"

void Model::Setup() {
  ModelLoader::GetInstance()->LoadModel(m_filename, Engine::GetActiveScene(), m_node, this);
  for (int i = 0; i < m_drawables.size(); i++) {
    m_drawables[i]->Setup();
  }
}

void Model::Reconfigure(){
  for (gfx::Pass* pass : m_passes) {
    for (Drawable* drawable : m_drawables) {
      bool found = false;
      for (gfx::Pass* p : drawable->m_material->GetPasses()) {
        if (p == pass) {
          found = true;
          break;
        }
      }
      if (!found) {
        drawable->m_material->AddPass(pass);
      }
    }
  }

  for (gfx::ResourceBindable* bind : m_binds) {
    for (Drawable* drawable : m_drawables) {
      bool found = false;
      for (gfx::ResourceBindable*b : drawable->m_material->GetBinds()){
        if (b == bind) {
          found = true;
          break;
        }
      }
      if (!found) {
        drawable->m_material->AddBindable(bind);
      }
    }
  }
}

Model::~Model() {
  //delete(m_animation);
}

void Model::AddDrawable(Drawable* drawable) {

  //Update BVH data
  if (!m_drawables.size()) {
    m_bvh = drawable->GetBVHData();
  }
  else {
    const BVHData& data = drawable->GetBVHData();

    m_bvh.m_min.x = std::min(m_bvh.m_min.x, data.m_min.x);
    m_bvh.m_min.y = std::min(m_bvh.m_min.y, data.m_min.y);
    m_bvh.m_min.z = std::min(m_bvh.m_min.z, data.m_min.z);

    m_bvh.m_max.x = std::max(m_bvh.m_max.x, data.m_max.x);
    m_bvh.m_max.y = std::max(m_bvh.m_max.y, data.m_max.y);
    m_bvh.m_max.z = std::max(m_bvh.m_max.z, data.m_max.z);
  }

  m_drawables.push_back(drawable);
}


const BVHData& Model::GetBVHData() const {
  return m_bvh;
}

void Model::AddPass(gfx::Pass* pass) {
  m_passes.push_back(pass);
}

void Model::AddBindable(gfx::ResourceBindable* bind) {
  m_binds.push_back(bind);
}

void Model::Insert(Node* node, const Transform& worldTransform) {
  if (m_hasAnimation) m_animation->Update();
}

REFLECT_STRUCT_BEGIN(Model, Entity)
REFLECT_STRUCT_MEMBER(m_filename)
REFLECT_STRUCT_MEMBER(m_passes)
REFLECT_STRUCT_MEMBER(m_binds)
REFLECT_STRUCT_END(Model)

REGISTER_ENTITY(Model)
