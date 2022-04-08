#ifndef __RESOURCE_BASE_H__
#define __RESOURCE_BASE_H__

#include "Globals.h"
#include <string>

class Resource;
enum class ResourceType;

class R_ResourceBase
{
public:
	R_ResourceBase(const Resource* r);
	~R_ResourceBase();

	bool CleanUp();

public:
	UID uid;
	ResourceType type;
	std::string assetsPath = "";
	std::string assetsFile = "";
	std::string libraryPath = "";
	std::string libraryFile = "";
};

#endif // !__RESOURCE_BASE_H__