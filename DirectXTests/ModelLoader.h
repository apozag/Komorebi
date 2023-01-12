#pragma once
#include <vector>
#include <string>

#include "Defines.h"
#include <DirectXMath.h>
#include "SimpleMath.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Graphics;
class Camera;
class SkinnedMesh;
class Pass;
class Scene;
class Skeleton;
class Model;
class Mesh;
class Animation;
class Node;
class Material;

class ModelLoader {
public:
	ModelLoader() = delete;
	static Model* LoadModel( std::string filename, Scene* sceneGraph, Node* sceneGraphParent);
	static void LoadModel( std::string filename, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static Mesh* GenerateMesh( std::vector<POD::Vertex> vertices, std::vector<unsigned short> indices);
	static Mesh* GenerateQuad(float scale = 1);
	static Mesh* GenerateCube(float scale = 1);
	static Mesh* GenerateAABB( DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max);
private:
	static void processNode( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static void processNodeBones( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static void processMaterials(const aiScene* scene);
	static Mesh* processMesh( aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent);
	static Animation* processAnimation(const aiScene* scene);
	static SkinnedMesh* processSkinnedMesh( aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);	

	static std::string directory;
	static std::vector<Material*> materials;
	static std::vector<Node*> boneNodes;
	static std::vector<std::string> boneNames;
	static std::vector<DirectX::XMMATRIX> boneOffsets;
};