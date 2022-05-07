#include "Importer.h"
#include "Engine.h"

Importer* Importer::instance = nullptr;

Importer::Importer(KoFiEngine* e)
{
	engine = e;
	sceneImporter = new I_Scene(e);
	meshImporter = new I_Mesh(e);
	materialImporter = new I_Material(e);
	textureImporter = new I_Texture(e);
	trackImporter = new I_Track(e);
	animationImporter = new I_Animations(e);
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