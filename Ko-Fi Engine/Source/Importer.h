#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "I_Material.h"
#include "I_Mesh.h"
#include "I_Scene.h"
#include "I_Texture.h"

class KofiEngine;

class Importer
{
private:
	Importer(KoFiEngine* e = nullptr);

public:
	~Importer();

	static Importer* GetInstance(KoFiEngine* e = nullptr);
	std::string GetNameFromPath(std::string path);

public:
	I_Scene* sceneImporter = nullptr;
	I_Mesh* meshImporter = nullptr;
	I_Material* materialImporter = nullptr;
	I_Texture* textureImporter = nullptr;

private:
	KoFiEngine* engine = nullptr;
	static Importer* instance;
};

#endif // !__IMPORTER_H__