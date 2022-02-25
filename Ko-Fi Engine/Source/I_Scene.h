#ifndef __I_SCENE_H__
#define __I_SCENE_H__

#include <vector>
#include "Globals.h"

class aiScene;
class aiNode;
class Scene;
class I_Mesh;
class GameObject;
class Mesh;

class I_Scene
{
public:
	I_Scene();
	~I_Scene();

	bool Import(const char* buffer, uint size, Mesh* mesh);
	bool Save(const Scene* scene, const char* path);
	bool Load(const char* path, Scene* scene);

	void ImportNode(const aiScene* assimpScene, const aiNode* assimpNode, Mesh* mesh, const Mesh& parent);
	//void ImportMeshesAndMaterials(const aiScene* assimpScene, const aiNode* assimpNode, Mesh* mesh, ModelNode& modelNode);
};

#endif // !__I_SCENE_H__