#ifndef __I_SCENE_H__
#define __I_SCENE_H__

#include "Globals.h"
#include <vector>
#include <string>

class KoFiEngine;
class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;
class Scene;
class GameObject;

class I_Scene
{
public:
	I_Scene(KoFiEngine* engine);
	~I_Scene();

	void CleanUp();
	bool Import(const char* path, bool isPrefab = false);
	bool Save(Scene* scene, const char* name = nullptr);
	bool Load(Scene* scene, const char* name);

	GameObject* ImportModel(const char* path);

	aiScene* GetAssimpScene();

private:
	void ImportNode(const aiScene* assimpScene, const aiNode* assimpNode, GameObject* parent, bool isPrefab = false);

	const aiNode* ImportTransform(const aiNode* assimpNode, GameObject* gameObj);
	bool IsDummyNode(const aiNode& assimpNode);

	void ImportMeshesAndMaterials(const aiScene* assimpScene, const aiNode* assimpNode, GameObject* gameObj);
	void ImportMesh(const char* nodeName, const aiMesh* assimpMesh, GameObject* gameObj, const aiScene* assimpScene = nullptr);
	void ImportMaterial(const char* nodeName, const aiMaterial* assimpMaterial, uint materialIndex, GameObject* gameObj);

private:
	KoFiEngine* engine = nullptr;

	std::string nodeName;

	// Instance of some generic Assimp variables in case we need them from the outside of the importer
	aiScene* assimpScene = nullptr;
};

#endif // !__I_SCENE_H__