#pragma once
#include "Module.h"

struct aiScene;
class GameObject;
class ComponentTransform;
class Mesh;
class I_Material;
class I_Mesh;
class I_Scene;
class I_Shader;
class I_Texture;

class Importer{
private:
	Importer();
public:
	~Importer();
	static Importer* GetInstance();
	GameObject* ImportModel(const char* path);
	bool SaveModel(const Mesh* mesh,const char* path);
	Mesh* LoadModel(const char* path);
	
	void SetEngine(KoFiEngine* engine) { this->engine = engine; }
	
private:
	ComponentTransform* AdjustTransform(const aiScene* scene, GameObject* go);
private:
	KoFiEngine* engine = nullptr;
	static Importer* instance;
public:
	I_Material* materialImporter = nullptr;
	I_Mesh* meshImporter = nullptr;
	I_Scene* sceneImporter = nullptr;
	I_Shader* shaderImporter = nullptr;
	I_Texture* textureImporter = nullptr;
};