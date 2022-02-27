#include "Importer.h"

Importer* Importer::instance = nullptr;

Importer::Importer()
{
	sceneImporter = new I_Scene(engine);
	meshImporter = new I_Mesh();
	materialImporter = new I_Material();
	textureImporter = new I_Texture();
	shaderImporter = new I_Shader();
}

Importer::~Importer()
{

}

Importer* Importer::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new Importer();
	}
	return instance;
}