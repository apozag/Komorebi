#include "SkinnedMesh.h"
#include "IndexBuffer.h"
#include "SkinnedVertexBuffer.h"
#include "ConstantBuffer.h"
#include "Node.h"
#include "Renderer.h"
#include "Skeleton.h"

SkinnedMesh::SkinnedMesh( std::vector<POD::SkinnedVertex>& vertices, std::vector<unsigned short>& indices, Skeleton* skeleton, BVHData bvhData) : Drawable (), m_skeleton(skeleton) {
    AddBindable(new SkinnedVertexBuffer ( vertices.data(), vertices.size(), sizeof(POD::SkinnedVertex), 0));
    AddIndexBuffer(new IndexBuffer ( indices.data(), indices.size()));
    m_bvhData = bvhData;
}

SkinnedMesh::SkinnedMesh(const SkinnedMesh& mesh) : Drawable((Drawable&)mesh), m_skeleton(mesh.m_skeleton){}

SkinnedMesh* SkinnedMesh::Clone() {
    return new SkinnedMesh(*this);
}
void SkinnedMesh::Draw(DirectX::XMMATRIX&& modelMatrix) const {
    if (m_skeleton->isDirty()) m_skeleton->Upload ();
    m_skeleton->Bind ();
    Drawable::Draw(std::move(modelMatrix));
}

