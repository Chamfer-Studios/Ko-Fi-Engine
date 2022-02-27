#ifndef __I_SCENE_H__
#define __I_SCENE_H__

#include "Globals.h"

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

	bool Import(const char* path);
	bool Save(const char* path);
	bool Load(const char* path);

	GameObject* ImportModel(const char* path);

private:
	void ImportNode(const aiScene* assimpScene, const aiNode* assimpNode, GameObject* parent);

	const aiNode* ImportTransform(const aiNode* assimpNode, GameObject* gameObj);
	bool IsDummyNode(const aiNode& assimpNode);

	void ImportMeshesAndMaterials(const aiScene* assimpScene, const aiNode* assimpNode, GameObject* gameObj);
	void ImportMesh(const char* nodeName, const aiMesh* assimpMesh, GameObject* gameObj);
	void ImportMaterial (const char* nodeName, const aiMaterial* assimpMaterial, uint materialIndex, GameObject* gameObj);

private:
	KoFiEngine* engine = nullptr;
};

#endif // !__I_SCENE_H__