#include "Importer.h"
#include "Engine.h"

Importer* Importer::instance = nullptr;

Importer::Importer(KoFiEngine* e)
{
	engine = e;
	sceneImporter = new I_Scene(e);
	meshImporter = new I_Mesh();
	materialImporter = new I_Material();
	textureImporter = new I_Texture();
}

Importer::~Importer()
{

}

Importer* Importer::GetInstance(KoFiEngine* e)
{
	if (instance == nullptr)
	{
		instance = new Importer(e);
	}
	return instance;
}