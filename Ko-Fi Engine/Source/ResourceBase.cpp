#include "ResourceBase.h"
#include "Resource.h"

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
