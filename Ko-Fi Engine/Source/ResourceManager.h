#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "Module.h"
#include "Globals.h"
#include "Resource.h"

class ResourceManager : public Module
{
public:
	ResourceManager(KoFiEngine* engine);
	~ResourceManager();

	bool Start();
	bool CleanUp();

	//uint Find(const char* assetPath) const;
	UID ImportFile(const char* assetPath);
	//uint GenerateNewUID();

	//const Resource* RequestResource(uint uid) const;
	Resource* RequestResource(UID uid);
	Resource::Type GetTypeFromExtension(const char* extension) const;

	//void ReleaseResource(uint uid);

private:
	Resource* CreateNewResource(const char* assetPath, Resource::Type type);

private:
	KoFiEngine* engine = nullptr;

	std::map<UID, Resource*> resourcesMap;
	std::map<UID, std::string> library;

	float fileRefreshRate;
};

#endif // __RESOURCE_MANAGER_H__