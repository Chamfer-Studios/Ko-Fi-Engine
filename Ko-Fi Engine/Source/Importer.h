#pragma once
#include "Module.h"

struct aiScene;

class Importer{
public:
	Importer(KoFiEngine* engine);
	~Importer();

	void ImportModel(const char* path);
private:
	void GetOneMesh(const aiScene* scene);
	void GetMultipleMeshes(const aiScene* scene);
private:
	KoFiEngine* engine = nullptr;

};