#pragma once

#define NOMINMAX

#include <utility>
#include <iostream>
#include <algorithm>
#include "ModelLoader.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Node.h"
#include "Scene.h"
#include "Skeleton.h"
#include "Bone.h"
#include "Pass.h"
#include "Rasterizer.h"
#include "DepthStencilState.h"
#include "Model.h"
#include "Animation.h"

std::string ModelLoader::directory;
std::vector<Node*> ModelLoader::boneNodes;
std::vector<std::string> ModelLoader::boneNames;
std::vector<DirectX::XMMATRIX> ModelLoader::boneOffsets;

DirectX::XMMATRIX aiMatrix4x4ToXMMATRIX(aiMatrix4x4 matrix) {
    return DirectX::XMMATRIX(        
        matrix.a1, matrix.b1, matrix.c1, matrix.d1,
        matrix.a2, matrix.b2, matrix.c2, matrix.d2,
        matrix.a3, matrix.b3, matrix.c3, matrix.d3,
        matrix.a4, matrix.b4, matrix.c4, matrix.d4
    );
}
Model* ModelLoader::LoadModel( std::string path, Scene* sceneGraph, Node* sceneGraphParent) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    Model* model = new Model ();
    Node* modelNode = sceneGraph->AddNode(model, Transform(), sceneGraphParent);
    directory = path.substr(0, path.find_last_of('/')+1);
    processNode ( scene->mRootNode, scene, sceneGraph, sceneGraphParent, model);

    boneNodes.resize(boneNames.size());

    processNodeBones ( scene->mRootNode, scene, sceneGraph, sceneGraphParent, model);
    if (scene->HasAnimations()) {
        model->m_hasAnimation = true;
        model->m_animation = processAnimation(scene);
    }

    boneNames.clear();
    boneOffsets.clear();
    boneNodes.clear();
    return model;
}

Mesh* ModelLoader::GenerateMesh( std::vector<POD::Vertex> vertices, std::vector<unsigned short> indices, Scene* sceneGraph, Node* sceneGraphParent) {
    POD::Vector3 min = vertices[0].pos, max = vertices[0].pos;
    for (int i = 1; i < vertices.size(); i++) {
        min.x = std::min(min.x, vertices[i].pos.x);
        min.y = std::min(min.y, vertices[i].pos.y);
        min.z = std::min(min.z, vertices[i].pos.z);

        max.x = std::max(max.x, vertices[i].pos.x);
        max.y = std::max(max.y, vertices[i].pos.y);
        max.z = std::max(max.z, vertices[i].pos.z);
    }
    Mesh* m = new Mesh ( vertices, indices, { {min.x, min.y, min.z}, {max.x, max.y, max.z} });
    sceneGraph->AddNode(m, Transform(), sceneGraphParent);
    return m;
}

Mesh* ModelLoader::GenerateQuad( Scene* sceneGraph, Node* sceneGraphParent, float scale) {
    std::vector<POD::Vertex> vertices {
        {{-1.0f*scale,  1.0f*scale, 0.5f*scale}, {0, 0, -1}, {0, 1, 0}, {0, 0}},
        {{ 1.0f*scale,  1.0f*scale, 0.5f*scale}, {0, 0, -1}, {0, 1, 0}, {1, 0}},
        {{ 1.0f*scale, -1.0f*scale, 0.5f*scale}, {0, 0, -1}, {0, 1, 0}, {1, 1}},
        {{-1.0f*scale, -1.0f*scale, 0.5f*scale}, {0, 0, -1}, {0, 1, 0}, {0, 1}}
    };

    std::vector<unsigned short> indices {
        0, 1, 2,
        0, 2, 3
    };

    return GenerateMesh ( vertices, indices, sceneGraph, sceneGraphParent);
}

Mesh* ModelLoader::GenerateCube( Scene* sceneGraph, Node* sceneGraphParent, float scale) {
    std::vector<POD::Vertex> vertices{
        {{-1.0f * scale,  1.0f * scale, -1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {0, 0}},
        {{ 1.0f * scale,  1.0f * scale, -1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {1, 0}},
        {{ 1.0f * scale, -1.0f * scale, -1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {1, 1}},
        {{-1.0f * scale, -1.0f * scale, -1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {0, 1}},

        {{-1.0f * scale,  1.0f * scale,  1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {0, 0}},
        {{ 1.0f * scale,  1.0f * scale,  1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {1, 0}},
        {{ 1.0f * scale, -1.0f * scale,  1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {1, 1}},
        {{-1.0f * scale, -1.0f * scale,  1.0f * scale}, {0, 0, 1}, {0, 1, 0}, {0, 1}}
    };

    std::vector<unsigned short> indices{
        0, 3, 2,    2, 1, 0,
        1, 2, 6,    6, 5, 1,
        6, 4, 5,    4, 6, 7,
        0, 4, 7,    7, 3, 0,
        1, 5, 4,    4, 0, 1,
        7, 6, 2,    2, 3, 7
    };

    return GenerateMesh ( vertices, indices, sceneGraph, sceneGraphParent);
} 

Mesh* ModelLoader::GenerateAABB( DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max, Scene* sceneGraph, Node* sceneGraphParent) {
    std::vector<POD::Vertex> vertices{
        {{min.x, max.y, min.z}, {0, 0, 1}, {0, 1, 0}, {0, 0}},
        {{max.x, max.y, min.z}, {0, 0, 1}, {0, 1, 0}, {1, 0}},
        {{max.x, min.y, min.z}, {0, 0, 1}, {0, 1, 0}, {1, 1}},
        {{min.x, min.y, min.z}, {0, 0, 1}, {0, 1, 0}, {0, 1}},

        {{min.x, max.y, max.z}, {0, 0, 1}, {0, 1, 0}, {0, 0}},
        {{max.x, max.y, max.z}, {0, 0, 1}, {0, 1, 0}, {1, 0}},
        {{max.x, min.y, max.z}, {0, 0, 1}, {0, 1, 0}, {1, 1}},
        {{min.x, min.y, max.z}, {0, 0, 1}, {0, 1, 0}, {0, 1}}
    };

    std::vector<unsigned short> indices{
        0, 3, 2,    2, 1, 0,
        1, 2, 6,    6, 5, 1,
        6, 4, 5,    4, 6, 7,
        0, 4, 7,    7, 3, 0,
        1, 5, 4,    4, 0, 1,
        7, 6, 2,    2, 3, 7
    };

    return GenerateMesh ( vertices, indices, sceneGraph, sceneGraphParent);
}

void ModelLoader::processNode( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model)
{

    Entity* entity = nullptr;

    Node* sceneNode = sceneGraph->AddNode(entity, Transform(aiMatrix4x4ToXMMATRIX(node->mTransformation)), sceneGraphParent);

    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        if (mesh->HasBones()) {
            model->AddDrawable(processSkinnedMesh ( mesh, scene, sceneGraph, sceneNode, model));
        }
        else {
            model->AddDrawable(processMesh ( mesh, scene, sceneGraph, sceneNode));
        }
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode ( node->mChildren[i], scene, sceneGraph, sceneNode, model);
    }
}

void ModelLoader::processNodeBones( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model) {
    Entity* entity = nullptr;

    const char* nodeName = node->mName.C_Str();
    int nodeIdx = -1;
    for (int i = 0; i < boneNames.size(); i++) {
        if (boneNames[i] == nodeName) {
            entity = new Bone(model->m_skeleton, i, boneOffsets[i]);
            nodeIdx = i;
            break;
        }
    }

    Node* sceneNode = sceneGraph->AddNode(entity, Transform(aiMatrix4x4ToXMMATRIX(node->mTransformation)), sceneGraphParent);
    
    if(nodeIdx > 0) boneNodes[nodeIdx] = sceneNode;

    Pass* aabbPass = new Pass ( "cubeVertex.cso", "SolidPixel.cso", PASSLAYER_OPAQUE);
    aabbPass->AddBindable(new Rasterizer ( true, true));
    aabbPass->AddBindable(new DepthStencilState (
        DepthStencilState::DepthStencilAccess::DEPTH_WRITE
    ));

    ModelLoader::GenerateCube ( sceneGraph, sceneNode)->AddPass(aabbPass);
        
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodeBones ( node->mChildren[i], scene, sceneGraph, sceneNode, model);
    }
}

Mesh* ModelLoader::processMesh( aiMesh* mesh, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent)
{
    std::vector<POD::Vertex> vertices(mesh->mNumVertices);
    std::vector<unsigned short> indices(mesh->mNumFaces * 3u);

    aiVector3D minVertex = mesh->mVertices[0], maxVertex = mesh->mVertices[0];

    // vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        POD::Vertex vertex;

        vertex.pos = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if (mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.uv = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }
        else vertex.uv = { 0.0f, 0.0f };

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = {
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z
            };
        }
        else vertex.uv = { 0.0f, 0.0f };

        vertices[i] = vertex;

        //Get the smallest vertex 
        minVertex.x = std::min(minVertex.x, vertex.pos.x); 
        minVertex.y = std::min(minVertex.y, vertex.pos.y); 
        minVertex.z = std::min(minVertex.z, vertex.pos.z); 

        //Get the largest vertex 
        maxVertex.x = std::max(maxVertex.x, vertex.pos.x); 
        maxVertex.y = std::max(maxVertex.y, vertex.pos.y); 
        maxVertex.z = std::max(maxVertex.z, vertex.pos.z); 
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices[i*3u + j] = face.mIndices[j];
    }
    Mesh* m = new Mesh ( vertices, indices, Drawable::BVHData{ {minVertex.x, minVertex.y, minVertex.z}, {maxVertex.x, maxVertex.y, maxVertex.z} });

    // material
    /*
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<aiTextureType> types = {
            aiTextureType_DIFFUSE, // t0
            aiTextureType_HEIGHT, // t1
            //aiTextureType_SPECULAR // t2      
        };
        for (int i = 0; i < types.size(); i++) {
            if (material->GetTextureCount(types[i]) == 0) continue;
            aiString str;
            material->GetTexture(types[i], 0, &str);
            m->AddBindable(new Texture2D ( this->directory + str.C_Str(), i));
        }
    }
    */
    sceneGraph->AddNode(m, Transform(), sceneGraphParent);

    return m;
}

SkinnedMesh* ModelLoader::processSkinnedMesh( aiMesh* mesh, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model)
{
    std::vector<POD::SkinnedVertex> vertices(mesh->mNumVertices);
    std::vector<unsigned short> indices(mesh->mNumFaces * 3u);

    aiVector3D minVertex = mesh->mVertices[0], maxVertex = mesh->mVertices[0];

    // vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        POD::SkinnedVertex vertex;

        vertex.pos = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if (mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.uv = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }
        else vertex.uv = { 0.0f, 0.0f };

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = {
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z
            };
        }
        else vertex.uv = { 0.0f, 0.0f };

        vertices[i] = vertex;

        //Get the smallest vertex 
        minVertex.x = std::min(minVertex.x, vertex.pos.x);
        minVertex.y = std::min(minVertex.y, vertex.pos.y);
        minVertex.z = std::min(minVertex.z, vertex.pos.z);

        //Get the largest vertex 
        maxVertex.x = std::max(maxVertex.x, vertex.pos.x);
        maxVertex.y = std::max(maxVertex.y, vertex.pos.y);
        maxVertex.z = std::max(maxVertex.z, vertex.pos.z);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices[i * 3u + j] = face.mIndices[j];
    }

    // bones
    int* boneFreeSlots = (int*)malloc(vertices.size() * sizeof(int));
    memset(boneFreeSlots, 0, vertices.size() * sizeof(int));

    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        boneNames.push_back(bone->mName.C_Str());
        boneOffsets.push_back(aiMatrix4x4ToXMMATRIX(bone->mOffsetMatrix));
        for (unsigned int j = 0; j < bone->mNumWeights; j++) {
            const aiVertexWeight& weight = bone->mWeights[j];
            int slot = boneFreeSlots[weight.mVertexId];
            vertices[weight.mVertexId].boneIdx[slot] = i;
            vertices[weight.mVertexId].weight[slot] = weight.mWeight;
            boneFreeSlots[weight.mVertexId]++;
        }
    }
    free(boneFreeSlots);

    SkinnedMesh* m = new SkinnedMesh ( vertices, indices, model->m_skeleton, Drawable::BVHData{ {minVertex.x, minVertex.y, minVertex.z}, {maxVertex.x, maxVertex.y, maxVertex.z} });

    // material
    /*
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<aiTextureType> types = {
            aiTextureType_DIFFUSE, // t0
            aiTextureType_HEIGHT, // t1
            aiTextureType_SPECULAR // t2      
        };
        for (int i = 0; i < types.size(); i++) {
            if (material->GetTextureCount(types[i]) == 0) continue;
            aiString str;
            material->GetTexture(types[i], 0, &str);
            m->AddBindable(new Texture2D ( this->directory + str.C_Str(), i));
        }
    }
    */
    sceneGraph->AddNode(m, Transform(), sceneGraphParent);

    return m;
}

Animation* ModelLoader::processAnimation(const aiScene* scene) {
    aiAnimation* animation = scene->mAnimations[0];

    double durationTicks = animation->mDuration;
    double ticksPerSecond = animation->mTicksPerSecond;

    std::vector<Animation::Channel> channels;

    // TODO: account for multiple animations

    for (int i = 0; i < animation->mNumChannels; i++) {
        aiNodeAnim* channel = animation->mChannels[i];

        Node* boneNode = nullptr;
        std::string boneName(channel->mNodeName.C_Str());
        for (int j = 0; j < boneNames.size(); j++) {
            if (boneName == boneNames[j]) {
                boneNode = boneNodes[j];
                break;
            }
        }

        if (boneNode == nullptr) continue;

        std::vector<Animation::Keyframe> keyframes(channel->mNumPositionKeys);
        for (int j = 0; j < channel->mNumPositionKeys; j++) {
            const aiVectorKey& pos = channel->mPositionKeys[j];
            const aiQuatKey& rot = channel->mRotationKeys[j];
            const aiVectorKey& scale = channel->mScalingKeys[j];
            keyframes[j] = {
                pos.mTime * 1000 / ticksPerSecond,
                {pos.mValue.x, pos.mValue.y, pos.mValue.z},
                {rot.mValue.x, rot.mValue.y, rot.mValue.z, rot.mValue.w},
                {scale.mValue.x, scale.mValue.y, scale.mValue.z},
            };
        }

        channels.push_back({
            std::move(keyframes),
            boneNode
        });

        
    }

    return new Animation(durationTicks, ticksPerSecond, std::move(channels));
}
