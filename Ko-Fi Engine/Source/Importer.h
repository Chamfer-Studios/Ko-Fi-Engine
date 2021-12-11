#pragma once
#include "Module.h"

struct aiScene;
class GameObject;
class ComponentTransform;
class Mesh;
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
	GameObject* GetOneMesh(const aiScene* scene);
	GameObject* GetMultipleMeshes(const aiScene* scene);

	ComponentTransform* AdjustTransform(const aiScene* scene, GameObject* go);
private:
	KoFiEngine* engine = nullptr;
	static Importer* instance;
};