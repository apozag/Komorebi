#define NOMINMAX

#include <algorithm>

#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Model.h"
#include "Skeleton.h"
#include "Animation.h"

Model::Model() {
    m_skeleton = new Skeleton ();    
}

void Model::AddDrawable(Drawable* drawable) {

    //Update BVH data
    if (!m_drawables.size()) {
        m_bvh = drawable->GetBVHData();
    }
    else {
        const Drawable::BVHData& data = drawable->GetBVHData();

        m_bvh.min.x = std::min(m_bvh.min.x, data.min.x);
        m_bvh.min.y = std::min(m_bvh.min.y, data.min.y);
        m_bvh.min.z = std::min(m_bvh.min.z, data.min.z);

        m_bvh.max.x = std::max(m_bvh.max.x, data.max.x);
        m_bvh.max.y = std::max(m_bvh.max.y, data.max.y);
        m_bvh.max.z = std::max(m_bvh.max.z, data.max.z);
    }

    m_drawables.push_back(drawable);
}

const Drawable::BVHData& Model::GetBVHData() const {    
    return m_bvh;
}

void Model::AddPass(Pass* pass){
    for (int i = 0; i < m_drawables.size(); i++)
        m_drawables[i]->AddPass(pass);
}

void Model::Insert(Node* node, const Transform& worldTransform) {
    if(m_hasAnimation) m_animation->Update();
}