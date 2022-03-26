#include "Resource.h"
#include "RNG.h"
#include "FSDefs.h"

Resource::Resource(ResourceType type) :
type(type),
uid(RNG::GetRandomUint()),
referenceCount(0),
assetPath(""),
assetFile(""),
libraryPath(""),
libraryFile("")
{

};

Resource::~Resource()
{
	CleanUp();
}

bool Resource::CleanUp()
{
	assetPath.clear();
	assetFile.clear();
	libraryPath.clear();
	libraryFile.clear();

	assetPath.shrink_to_fit();
	assetFile.shrink_to_fit();
	libraryPath.shrink_to_fit();
	libraryFile.shrink_to_fit();
	return true;
}

void Resource::SetLibraryPathAndFile()
{
	std::string dir = "";
	std::string file = std::to_string(uid);
	std::string extension = "";

	switch (type)
	{
	case ResourceType::MESH:
		dir = MESHES_DIR;
		extension = MESH_EXTENSION;
		break;
	case ResourceType::TEXTURE:
		dir = TEXTURES_DIR;
		extension = TEXTURE_EXTENSION;
		break;
	case ResourceType::SCENE:
		dir = ASSETS_SCENES_DIR;
		extension = SCENE_EXTENSION;
		break;
	case ResourceType::FONT:
		dir = FONT_DIR;
		extension = FONT_EXTENSION;
		break;
	case ResourceType::UNKNOWN:
		break;
	default:
		break;
	}

	libraryPath = dir + file + extension;
	libraryFile = file + extension;
}

void Resource::SetAssetsPathAndFile(const char* path,const char* file)
{
	assetPath = path;
	assetFile = file;
}

//bool Resource::HasResource(UID uid) const
//{
//	return false;
//}