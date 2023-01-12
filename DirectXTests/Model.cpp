#define NOMINMAX

#include <algorithm>

#include "Engine.h"
#include "Scene.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Model.h"
#include "Skeleton.h"
#include "Animation.h"
#include "Material.h"

#include "ModelLoader.h"

void Model::Setup() {
  ModelLoader::LoadModel(m_filename, Engine::m_activeScene, Engine::m_activeScene->GetRootNode(), this);
}

void Model::AddDrawable(Drawable* drawable) {

  //Update BVH data
  if (!m_drawables.size()) {
    m_bvh = drawable->GetBVHData();
  }
  else {
    const Drawable::BVHData& data = drawable->GetBVHData();

    m_bvh.m_min.x = std::min(m_bvh.m_min.x, data.m_min.x);
    m_bvh.m_min.y = std::min(m_bvh.m_min.y, data.m_min.y);
    m_bvh.m_min.z = std::min(m_bvh.m_min.z, data.m_min.z);

    m_bvh.m_max.x = std::max(m_bvh.m_max.x, data.m_max.x);
    m_bvh.m_max.y = std::max(m_bvh.m_max.y, data.m_max.y);
    m_bvh.m_max.z = std::max(m_bvh.m_max.z, data.m_max.z);
  }

  m_drawables.push_back(drawable);
}


const Drawable::BVHData& Model::GetBVHData() const {
  return m_bvh;
}

void Model::AddPass(Pass* pass) {
  for (int i = 0; i < m_drawables.size(); i++)
    m_drawables[i]->m_material->AddPass(pass);
}

void Model::AddBindable(ResourceBindable* bind) {
  for (int i = 0; i < m_drawables.size(); i++)
    m_drawables[i]->m_material->AddBindable(bind);
}

void Model::Insert(Node* node, const Transform& worldTransform) {
  if (m_hasAnimation) m_animation->Update();
}

REFLECT_STRUCT_BEGIN(Model, Entity)
REFLECT_STRUCT_MEMBER(m_filename)
REFLECT_STRUCT_END(Model)