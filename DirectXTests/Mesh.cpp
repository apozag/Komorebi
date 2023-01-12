#include "Mesh.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Node.h"
#include "Renderer.h"
/*
Mesh Mesh::s_quad(
    {
        { {-1.0f,  1.0f, 0.5f}, {0, 0, -1}, { 0, 1, 0}, {0, 0}},
        { { 1.0f,  1.0f, 0.5f}, {0, 0, -1}, {0, 1, 0}, {1, 0} },
        { { 1.0f, -1.0f, 0.5f}, {0, 0, -1}, {0, 1, 0}, {1, 1} },
        { {-1.0f, -1.0f, 0.5f}, {0, 0, -1}, {0, 1, 0}, {0, 1} }
    },
    {
        0, 1, 2,
        0, 2, 3
    },
    { {-1, -1, 0.5f}, {1, 1, 0.5f} }
);
*/
Mesh::Mesh( std::vector<POD::Vertex>& vertices, std::vector<unsigned short>& indices, BVHData bvhData): Drawable () {
    AddBindable(new VertexBuffer ( vertices.data(), vertices.size(), sizeof(POD::Vertex), 0));
    AddIndexBuffer(new IndexBuffer ( indices.data(), indices.size()));
    m_bvhData = bvhData;
}

Mesh::Mesh(const Mesh& mesh) : Drawable((Drawable&)mesh){}

Mesh* Mesh::Clone() {
    return new Mesh(*this);
}

REFLECT_STRUCT_BEGIN(Mesh, Drawable)
REFLECT_STRUCT_END(Mesh)
