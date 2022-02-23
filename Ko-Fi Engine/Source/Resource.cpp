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

void Resource::CleanUp()
{
	assetPath.clear();
	libraryPath.clear();

	assetPath.shrink_to_fit();
	libraryPath.shrink_to_fit();
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
		// TODO
		break;
	case Resource::Type::SCENE:
		dir = SCENES_DIR;
		extension = ".json";
		break;
	case Resource::Type::SHADER:
		dir = SHADERS_DIR;
		extension = SHADER_EXTENSION;
		break;
	case Resource::Type::UNKNOWN:
		break;
	default:
		break;
	}

	libraryPath = dir + file + extension;
	libraryFile = file + extension;
}

//bool Resource::HasResource(UID uid) const
//{
//	return false;
//}