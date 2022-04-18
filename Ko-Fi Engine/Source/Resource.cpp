#include "Resource.h"
#include "RNG.h"
#include "FSDefs.h"

Resource::Resource() :
type(ResourceType::UNKNOWN),
uid(0),
referenceCount(0),
assetPath(""),
assetFile(""),
libraryPath(""),
libraryFile("")
{
}

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

void Resource::ForceUID(const UID& uid)
{
	this->uid = uid;
	SetLibraryPathAndFile();
}

void Resource::ModifyReferenceCount(int modification)
{
	int modifiedReferences = referenceCount + modification;
	referenceCount = (modifiedReferences < 0) ? 0 : modifiedReferences;
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
	//case ResourceType::SCENE:
	//	dir = SCENES_DIR;
	//	extension = SCENE_EXTENSION;
	//	break;
	//case ResourceType::FONT:
	//	dir = FONTS_DIR;
	//	extension = FONT_EXTENSION;
	//	break;
	//case ResourceType::TRACK:
	//	dir = AUDIOS_DIR;
	//	extension = AUDIO_EXTENSION;
	//	break;
	//case ResourceType::PARTICLE:
	//	dir = FONTS_DIR;
	//	extension = FONT_EXTENSION;
	//	break;
	case ResourceType::MODEL:
		dir = MODELS_DIR;
		extension = MODEL_EXTENSION;
		break;
	case ResourceType::MATERIAL:
		dir = SHADERS_DIR;
		extension = SHADER_EXTENSION;
		break;
	case ResourceType::ANIMATION:
		dir = ANIMATIONS_DIR;
		extension = ANIMATION_EXTENSION;
		break;
	case ResourceType::UNKNOWN:
		break;
	default:
		break;
	}

	libraryPath = dir + file + extension;
	libraryFile = file + extension;
}

void Resource::SetAssetsPathAndFile(const char* path, const char* file)
{
	assetPath = path;
	assetFile = file;
}

//bool Resource::HasResource(UID uid) const
//{
//	return false;
//}