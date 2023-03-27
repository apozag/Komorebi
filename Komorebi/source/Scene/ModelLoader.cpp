#pragma once

#define NOMINMAX

#include <utility>
#include <iostream>
#include <algorithm>

#include "Core/Memory/Factory.h"
#include "Scene/ModelLoader.h"
#include "Graphics/Bindables/Resource/Texture2D.h"
#include "Graphics/Bindables/State/RasterizerState.h"
#include "Entities/Mesh.h"
#include "Entities/SkinnedMesh.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"
#include "Animation/Skeleton.h"
#include "Entities/Bone.h"
#include "Graphics/Pass.h"
#include "Graphics/Bindables/State/SamplerState.h"
#include "Graphics/Bindables/State/DepthStencilState.h"
#include "Entities/Model.h"
#include "Animation/Animation.h"
#include "Graphics/Material.h"
#include "Graphics/BindableSlotsInfo.h"
#include "Core/Util/ImageManager.h"

DirectX::XMMATRIX aiMatrix4x4ToXMMATRIX(aiMatrix4x4 matrix) {
  return DirectX::XMMATRIX(
    matrix.a1, matrix.b1, matrix.c1, matrix.d1,
    matrix.a2, matrix.b2, matrix.c2, matrix.d2,
    matrix.a3, matrix.b3, matrix.c3, matrix.d3,
    matrix.a4, matrix.b4, matrix.c4, matrix.d4
  );
}

int boneCount = 0;

void ModelLoader::LoadModel(std::string filename, Scene* sceneGraph, Node* sceneGraphParent, Model* model) {

  if (filename == "cube") {
    Mesh* mesh = GenerateCube();
    mesh->m_material = memory::Factory::Create<Material>();
    for (Pass* pass : model->GetPasses()) {
      mesh->m_material->AddPass(pass);
    }
    for (ResourceBindable* bind : model->GetBinds()) {
      mesh->m_material->AddBindable(bind);
    }
    sceneGraph->AddNode(mesh, Transform(), sceneGraphParent, true);
    model->AddDrawable(mesh);
    return;
  }

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    return;
  }

  directory = filename.substr(0, filename.find_last_of('/') + 1);

  processMaterials(scene);

  for (Material* mat : materials) {
    for (Pass* pass : model->GetPasses()) {
      mat->AddPass(pass);
    }
    for (ResourceBindable* bind : model->GetBinds()) {
      mat->AddBindable(bind);
    }
  }

  processNode(scene->mRootNode, scene, sceneGraph, sceneGraphParent, model);

  boneNodes.resize(boneNames.size());

  processNodeBones(scene->mRootNode, scene, sceneGraph, sceneGraphParent, model);

  if (model->m_hasAnimation = scene->HasAnimations()) {
    model->m_animation = processAnimation(scene);
  }

  materials.clear();
  boneNames.clear();
  boneOffsets.clear();
  boneNodes.clear();
}

Model* ModelLoader::LoadModel(std::string path, Scene* sceneGraph, Node* sceneGraphParent) {
  Model* model = memory::Factory::Create<Model>();
  if (path == "cube") {
    Mesh* mesh = GenerateCube();
    mesh->m_material = memory::Factory::Create<Material>();
    sceneGraph->AddNode(mesh, Transform(), sceneGraphParent, true);
    model->AddDrawable(mesh);
  }
  else {
    LoadModel(path, sceneGraph, sceneGraphParent, model);
  }
  return model;
}

Mesh* ModelLoader::GenerateMesh(std::vector<POD::Vertex> vertices, std::vector<unsigned short> indices) {
  POD::Vector3 min = vertices[0].pos, max = vertices[0].pos;
  for (unsigned int i = 1; i < vertices.size(); i++) {
    min.x = std::min(min.x, vertices[i].pos.x);
    min.y = std::min(min.y, vertices[i].pos.y);
    min.z = std::min(min.z, vertices[i].pos.z);

    max.x = std::max(max.x, vertices[i].pos.x);
    max.y = std::max(max.y, vertices[i].pos.y);
    max.z = std::max(max.z, vertices[i].pos.z);
  }
  Drawable::BVHData bvhData{ {min.x, min.y, min.z}, {max.x, max.y, max.z} };
  Mesh* m = memory::Factory::Create<Mesh>(vertices, indices, bvhData);
  return m;
}

Mesh* ModelLoader::GenerateQuad(float scale) {
  std::vector<POD::Vertex> vertices{
      {{-1.0f * scale,  1.0f * scale, 0.5f * scale}, {0, 0, -1}, {0, 1, 0}, {0, 0}},
      {{ 1.0f * scale,  1.0f * scale, 0.5f * scale}, {0, 0, -1}, {0, 1, 0}, {1, 0}},
      {{ 1.0f * scale, -1.0f * scale, 0.5f * scale}, {0, 0, -1}, {0, 1, 0}, {1, 1}},
      {{-1.0f * scale, -1.0f * scale, 0.5f * scale}, {0, 0, -1}, {0, 1, 0}, {0, 1}}
  };

  std::vector<unsigned short> indices{
      0, 1, 2,
      0, 2, 3
  };

  return GenerateMesh(vertices, indices);
}

Mesh* ModelLoader::GenerateCube(float scale) {
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

  return GenerateMesh(vertices, indices);
}

Mesh* ModelLoader::GenerateAABB(DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max) {
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

  return GenerateMesh(vertices, indices);
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model)
{
  Node* sceneNode = sceneGraphParent;
  if (node->mNumMeshes > 0) {
    sceneNode = sceneGraph->AddNode(nullptr, Transform(aiMatrix4x4ToXMMATRIX(node->mTransformation)), sceneGraphParent, true);
  }

  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    if (mesh->HasBones()) {
      model->AddDrawable(processSkinnedMesh(mesh, scene, sceneGraph, sceneNode, model));
    }
    else {
      model->AddDrawable(processMesh(mesh, scene, sceneGraph, sceneNode));
    }
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNode(node->mChildren[i], scene, sceneGraph, sceneNode, model);
  }
}

void ModelLoader::processNodeBones(aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model) {
  Entity* entity = nullptr;

  const char* nodeName = node->mName.C_Str();
  int nodeIdx = -1;
  for (int i = 0; i < boneNames.size(); i++) {
    if (boneNames[i] == nodeName) {
      entity = memory::Factory::Create<Bone>(&model->m_skeleton, i, boneOffsets[i]);
      nodeIdx = i;
      break;
    }
  }

  Node* sceneNode = nullptr;

  if (nodeIdx >= 0) {
    sceneNode = sceneGraph->AddNode(entity, Transform(aiMatrix4x4ToXMMATRIX(node->mTransformation)), sceneGraphParent, true);
    boneNodes[nodeIdx] = sceneNode;
    boneCount++;
  }
  else {
    sceneNode = sceneGraphParent;
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNodeBones(node->mChildren[i], scene, sceneGraph, sceneNode, model);
  }
}

void ModelLoader::processMaterials(const aiScene* scene) {

  for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
    aiMaterial* material = scene->mMaterials[i];
    Material* mat = memory::Factory::Create<Material>();
    std::vector<aiTextureType> types = {
        aiTextureType_DIFFUSE, // t0
        aiTextureType_HEIGHT, // t1
        aiTextureType_SPECULAR // t2      
    };
    for (int j = 0; j < types.size(); j++) {
      if (material->GetTextureCount(types[j]) == 0) continue;
      aiString str;
      material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), str);
      if (const aiTexture* texture = scene->GetEmbeddedTexture(str.C_Str())) {
        //returned pointer is not null, read texture from memory
        if (texture->mHeight == 0) {
          // Compressed image data, we need to decode it
          Image img = ImageManager::decodeFromMemory(texture->mFilename.C_Str(), (unsigned char*)texture->pcData, texture->mWidth);
          mat->AddBindable(memory::Factory::Create < Texture2D>(img.data, img.width, img.height, img.channels, SRV_FREE_SLOT + j));
        }
        else {
          mat->AddBindable(memory::Factory::Create < Texture2D>((unsigned char*)texture->pcData, texture->mWidth, texture->mHeight, 4, SRV_FREE_SLOT + j));
        }
      }
      else {
        //regular file, read it from disk
        mat->AddBindable(memory::Factory::Create < Texture2D>(directory + str.C_Str(), SRV_FREE_SLOT + j));
      }

    }
    mat->AddBindable(memory::Factory::Create<SamplerState>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, SRV_FREE_SLOT));
    materials.push_back(mat);
  }
}

Mesh* ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent)
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
      indices[i * 3u + j] = face.mIndices[j];
  }
  Mesh* m = memory::Factory::Create<Mesh>(vertices, indices, Drawable::BVHData{ {minVertex.x, minVertex.y, minVertex.z}, {maxVertex.x, maxVertex.y, maxVertex.z} });
  m->m_material = materials[mesh->mMaterialIndex];
  sceneGraph->AddNode(m, Transform(), sceneGraphParent, true);

  return m;
}

SkinnedMesh* ModelLoader::processSkinnedMesh(aiMesh* mesh, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model)
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
    std::string boneName = bone->mName.C_Str();
    // Search bone by name, add if not present
    int bone_idx;
    for (bone_idx = 0; bone_idx < boneNames.size(); bone_idx++) {
      if (boneNames[bone_idx] == boneName) break;
    }
    if (bone_idx == boneNames.size()) {
      boneNames.push_back(boneName);
      boneOffsets.push_back(aiMatrix4x4ToXMMATRIX(bone->mOffsetMatrix));
    }
    for (unsigned int j = 0; j < bone->mNumWeights; j++) {
      const aiVertexWeight& weight = bone->mWeights[j];
      int slot = boneFreeSlots[weight.mVertexId];
      vertices[weight.mVertexId].boneIdx[slot] = bone_idx;
      vertices[weight.mVertexId].weight[slot] = weight.mWeight;
      boneFreeSlots[weight.mVertexId]++;
    }
  }

  free(boneFreeSlots);

  SkinnedMesh* m = memory::Factory::Create<SkinnedMesh>(vertices, indices, &model->m_skeleton, Drawable::BVHData{ {minVertex.x, minVertex.y, minVertex.z}, {maxVertex.x, maxVertex.y, maxVertex.z} });
  m->m_material = materials[mesh->mMaterialIndex];
  Node* meshNode = sceneGraph->AddNode(m, Transform(), sceneGraphParent, true);

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
    for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
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

  return memory::Factory::Create<Animation>(durationTicks, ticksPerSecond, std::move(channels));
}
