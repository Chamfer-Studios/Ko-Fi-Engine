#include "ResourceBase.h"
#include "Resource.h"
#include <string>

ResourceBase::ResourceBase(UID uid, const ResourceType& type, const std::string& assetsPath, const std::string& assetsFile, const std::string& libraryPath, const std::string& libraryFile) :
uid(uid),
type(type),
assetPath(assetsPath),
assetFile(assetsFile),
libraryPath(libraryPath),
libraryFile(libraryFile)
{}

ResourceBase::ResourceBase(const Resource* r)
{
	if (r != nullptr)
	{
		uid = r->GetUID();
		type = r->GetType();
		assetPath = r->GetAssetPath();
		assetFile = r->GetAssetFile();
		libraryPath = r->GetLibraryPath();
		libraryFile = r->GetLibraryFile();
	}
	else
	{
		uid = 0;
		type = ResourceType::UNKNOWN;
		assetPath = "";
		assetFile = "";
		libraryPath = "";
		libraryFile = "";
	}
}

ResourceBase::~ResourceBase()
{
	CleanUp();
}

bool ResourceBase::CleanUp()
{
	assetPath.clear();
	assetPath.shrink_to_fit();
	assetFile.clear();
	assetFile.shrink_to_fit();
	libraryPath.clear();
	libraryPath.shrink_to_fit();
	libraryFile.clear();
	libraryFile.shrink_to_fit();
	return true;
}
