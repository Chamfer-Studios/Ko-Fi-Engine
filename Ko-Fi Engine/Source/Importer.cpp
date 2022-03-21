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
	trackImporter = new I_Track();
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

std::string Importer::GetNameFromPath(std::string path)
{
	std::string name = path;
	name = name.substr(name.find_last_of("/\\") + 1);
	std::string::size_type const p(name.find_last_of('.'));
	name = name.substr(0, p);
	return name;
}
