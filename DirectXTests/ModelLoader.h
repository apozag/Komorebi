#pragma once
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Defines.h"

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

class ModelLoader {
public:
	ModelLoader() = delete;
	static Model* LoadModel( std::string filename, Scene* sceneGraph, Node* sceneGraphParent);
	static Mesh* GenerateMesh( std::vector<POD::Vertex> vertices, std::vector<unsigned short> indices, Scene* sceneGraph, Node* sceneGraphParent);
	static Mesh* GenerateQuad( Scene* sceneGraph, Node* sceneGraphParent, float scale = 1);
	static Mesh* GenerateCube( Scene* sceneGraph, Node* sceneGraphParent, float scale = 1);
	static Mesh* GenerateAABB( DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max, Scene* sceneGraph, Node* sceneGraphParent);
private:
	static void processNode( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static void processNodeBones( aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static Mesh* processMesh( aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent);
	static Animation* processAnimation(const aiScene* scene);
	static SkinnedMesh* processSkinnedMesh( aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static std::string directory;

	static std::vector<Node*> boneNodes;
	static std::vector<std::string> boneNames;
	static std::vector<DirectX::XMMATRIX> boneOffsets;
};