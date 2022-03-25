#ifndef __RESOURCE_BASE_H__
#define __RESOURCE_BASE_H__

#include "Globals.h"
#include <string>

class Resource;
enum class ResourceType;

class ResourceBase
{
public:
	ResourceBase(const Resource* r);
	~ResourceBase();

	bool CleanUp();

public:
	UID uid;
	ResourceType type;
	std::string assetsPath;
	std::string assetsFile;
	std::string libraryPath;
	std::string libraryFile;
};

#endif // !__RESOURCE_BASE_H__