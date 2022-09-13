#include "Mesh.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Node.h"
#include "Renderer.h"

Mesh::Mesh( std::vector<POD::Vertex>& vertices, std::vector<unsigned short>& indices, BVHData bvhData): Drawable () {
    AddBindable(new VertexBuffer ( vertices.data(), vertices.size(), sizeof(POD::Vertex), 0));
    AddIndexBuffer(new IndexBuffer ( indices.data(), indices.size()));
    m_bvhData = bvhData;
}

Mesh::Mesh(const Mesh& mesh) : Drawable((Drawable&)mesh){}

Mesh* Mesh::Clone() {
    return new Mesh(*this);
}
