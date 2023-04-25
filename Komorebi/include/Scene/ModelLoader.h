#pragma once
#include <vector>
#include <string>

#include "Core/Defines.h"
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "Core/Singleton.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace gfx {
	class Graphics;
	class Material;
	class MaterialInstance;
	class Pass;
}

class Camera;
class SkinnedMesh;
class Scene;
class Skeleton;
class Model;
class Mesh;
class Animation;
class Node;

class ModelLoader : public Singleton<ModelLoader> {
public:
	Model* LoadModel( std::string filename, Scene* sceneGraph, Node* sceneGraphParent);
	void LoadModel( std::string filename, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	Mesh* GenerateMesh( std::vector<POD::Vertex> vertices, std::vector<unsigned short> indices);
	Mesh* GenerateQuad(float scale = 1);
	Mesh* GenerateCube(float scale = 1);
	Mesh* GenerateAABB( DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max);
private:
	void processNode( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	void processNodeBones( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	void processMaterials(const aiScene* scene, const gfx::Material* material);
	Mesh* processMesh( aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent);
	Animation* processAnimation(const aiScene* scene);
	SkinnedMesh* processSkinnedMesh( aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);	

	std::string directory;
	std::vector<gfx::MaterialInstance*> matInstances;
	std::vector<Node*> boneNodes;
	std::vector<std::string> boneNames;
	std::vector<DirectX::XMMATRIX> boneOffsets;
};