#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "I_Material.h"
#include "I_Mesh.h"
#include "I_Scene.h"
#include "I_Shader.h"
#include "I_Texture.h"

class Importer
{
private:
	Importer();

public:
	~Importer();

	static Importer* GetInstance();

	inline void SetEngine(KoFiEngine* engine) { this->engine = engine; }

public:
	I_Scene* sceneImporter = nullptr;
	I_Mesh* meshImporter = nullptr;
	I_Material* materialImporter = nullptr;
	I_Texture* textureImporter = nullptr;
	I_Shader* shaderImporter = nullptr;

private:
	KoFiEngine* engine = nullptr;
	static Importer* instance;
};

#endif // !__IMPORTER_H__