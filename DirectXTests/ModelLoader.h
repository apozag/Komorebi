#pragma once
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Defines.h"
#include "ConstantBuffer.h"
#include "Drawable.h"
#include "Mesh.h"

class Graphics;
class Camera;
class SkinnedMesh;
class Pass;
class Scene;
class Skeleton;
class Model;
class Animation;
class Node;

class ModelLoader {
public:
	ModelLoader() = delete;
	static Model* LoadModel(Graphics& gfx, std::string filename, Scene* sceneGraph, Node* sceneGraphParent);
	static Mesh* GenerateMesh(Graphics& gfx, std::vector<POD::Vertex> vertices, std::vector<unsigned short> indices, Scene* sceneGraph, Node* sceneGraphParent);
	static Mesh* GenerateQuad(Graphics& gfx, Scene* sceneGraph, Node* sceneGraphParent, float scale = 1);
	static Mesh* GenerateCube(Graphics& gfx, Scene* sceneGraph, Node* sceneGraphParent, float scale = 1);
	static Mesh* GenerateAABB(Graphics& gfx, DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max, Scene* sceneGraph, Node* sceneGraphParent);
private:
	static void processNode(Graphics& gfx, aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static void processNodeBones(Graphics& gfx, aiNode* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static Mesh* processMesh(Graphics& gfx, aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent);
	static Animation* processAnimation(const aiScene* scene);
	static SkinnedMesh* processSkinnedMesh(Graphics& gfx, aiMesh* node, const aiScene* scene, Scene* sceneGraph, Node* sceneGraphParent, Model* model);
	static std::string directory;

	static std::vector<Node*> boneNodes;
	static std::vector<std::string> boneNames;
	static std::vector<DirectX::XMMATRIX> boneOffsets;
};