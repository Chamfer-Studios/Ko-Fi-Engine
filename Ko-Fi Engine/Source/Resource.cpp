#include "Resource.h"
#include "RNG.h"
#include "FSDefs.h"

Resource::Resource(Resource::Type type) :
type(type),
uid(RNG::GetRandomUint()),
referenceCount(0),
assetPath(""),
libraryPath("")
{

};

Resource::~Resource()
{

}

bool Resource::CleanUp()
{
	assetPath.clear();
	libraryPath.clear();

	assetPath.shrink_to_fit();
	libraryPath.shrink_to_fit();
	return true;
}

void Resource::SetLibraryPathAndFile()
{
	std::string dir = "";
	std::string file = std::to_string(uid);
	std::string extension = "";

	switch (type)
	{
	case Resource::Type::MESH:
		dir = MESHES_DIR;
		extension = MESH_EXTENSION;
		break;
	case Resource::Type::TEXTURE:
		dir = TEXTURES_DIR;
		extension = TEXTURE_EXTENSION;
		break;
	case Resource::Type::SCENE:
		dir = ASSETS_SCENES_DIR;
		extension = SCENE_EXTENSION;
		break;
	case Resource::Type::SHADER:
		dir = SHADERS_DIR;
		extension = SHADER_EXTENSION;
		break;
	case Resource::Type::FONT:
		dir = FONT_DIR;
		extension = FONT_EXTENSION;
		break;
	case Resource::Type::UNKNOWN:
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