#include "ResourceBase.h"
#include "Resource.h"
#include <string>

ResourceBase::ResourceBase(UID uid, const ResourceType& type, const std::string& assetsPath, const std::string& assetsFile, const std::string& libraryPath, const std::string& libraryFile) :
uid(uid),
type(type),
assetsPath(assetsPath),
assetsFile(assetsFile),
libraryPath(libraryPath),
libraryFile(libraryFile)
{}

ResourceBase::ResourceBase(const Resource* r)
{
	if (r != nullptr)
	{
		uid = r->GetUID();
		type = r->GetType();
		assetsPath = r->GetAssetPath();
		assetsFile = r->GetAssetFile();
		libraryPath = r->GetLibraryPath();
		libraryFile = r->GetLibraryFile();
	}
	else
	{
		uid = 0;
		type = ResourceType::UNKNOWN;
		assetsPath = "";
		assetsFile = "";
		libraryPath = "";
		libraryFile = "";
	}
}

ResourceBase::~ResourceBase()
{
}

bool ResourceBase::CleanUp()
{
	return true;
}
