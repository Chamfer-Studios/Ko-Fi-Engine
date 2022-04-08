#include "R_ResourceBase.h"
#include "Resource.h"
#include <string>

R_ResourceBase::R_ResourceBase(const Resource* r)
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

R_ResourceBase::~R_ResourceBase()
{
}

bool R_ResourceBase::CleanUp()
{
	return true;
}
