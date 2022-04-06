#include "ResourceManager.h"
#include "Engine.h"
#include "Log.h"

ResourceManager::ResourceManager(KoFiEngine* engine) : Module(),
fileRefreshRate(0.0f),
fileRefreshTime(0.0f)
{
	name = "ResourceManager";
	this->engine = engine;
}

ResourceManager::~ResourceManager()
{

}

bool ResourceManager::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool ResourceManager::Start()
{
	CONSOLE_LOG("Starting ResourceManager...");
	//appLog->AddLog("Starting ResourceManager...\n");

	fileRefreshRate = 5.0f;

	//RefreshDirectoryFiles()

	return true;
}

bool ResourceManager::PreUpdate(float dt)
{
	//fileRefreshTime += dt;
	//if (fileRefreshTime > fileRefreshRate)
	//{
	//	std::map<UID, Resource*>::iterator it = resourcesMap.begin();
	//	while (it != resourcesMap.end())
	//	{
	//		if (it->second->GetReferenceCount() == 0)
	//		{
	//			UID resourceUID = it->second->GetUID();
	//			++it;
	//			DeallocateResource(resourceUID);
	//			continue;
	//		}
	//		++it;
	//	}
	//	fileRefreshTime = 0.0f;
	//}

	return true;
}

bool ResourceManager::CleanUp()
{
	CONSOLE_LOG("Cleaning ResourceManager up...");
	//appLog->AddLog("Cleaning ResourceManager up...\n");

	bool ret = true;

	std::map<UID, Resource*>::iterator it;
	for (it = resourcesMap.begin(); it != resourcesMap.end(); ++it)
	{
		it->second->CleanUp();
		RELEASE(it->second);
	}

	resourcesMap.clear();
	library.clear();

	return ret;
}

UID ResourceManager::ImportFile(const char* assetPath)
{
	// TODO: Get type based on file extension

	Resource* resource = CreateNewResource(assetPath, Resource::Type::MESH);
	UID ret = 0; // What will be returned

	// TODO: Load from filesystem
	//char* fileBuffer = engine->filesystem->Load(assetPath);

	switch (resource->GetType())
	{
	case Resource::Type::MESH:
		// TODO: Import Mesh
		break;
	case Resource::Type::TEXTURE:
		// TODO: Import Texture
		break;
	case Resource::Type::SCENE:
		// TODO: Import Scene
		break;
	case Resource::Type::SHADER:
		// TODO: Import Shader
		break;
	case Resource::Type::UNKNOWN:
		break;
	default:
		break;
	}

	// TODO: Save the Resource
	//SaveResource(resource);

	// Get the UID Resource to return
	ret = resource->GetUID();

	// TODO: Unload the resource after importing, we should only use the ID
	//RELEASE_ARRAY(fileBuffer);
	//UnloadResource(resource);

	return ret;
}

Resource* ResourceManager::CreateNewResource(const char* assetPath, Resource::Type type)
{
	Resource* ret = nullptr;
	UID uid = 0; // uid = GenerateNewUID()

	switch (type)
	{
	case Resource::Type::MESH:
		// TODO: Create Mesh Resource
		break;
	case Resource::Type::TEXTURE:
		// TODO: Create Texture Resource
		break;
	case Resource::Type::SCENE:
		// TODO: Create Scene Resource
		break;
	case Resource::Type::SHADER:
		// TODO: Create Shader Resource
		break;
	case Resource::Type::UNKNOWN:
		break;
	default:
		break;
	}

	if (ret != nullptr)
	{
		// TODO: Put the created resource in the map
		//resourcesMap[uid] = ret;

		// TODO: Save assetPath and generate a library path
		//resource->assetPath = assetPath;
		//resource->libraryPath = GenerateLibraryPath(resource);
	}

	return ret;
}

Resource* ResourceManager::RequestResource(UID uid)
{
	// Find if the resource is already loaded
	std::map<UID, Resource*>::iterator it = resourcesMap.find(uid);

	if (it != resourcesMap.end()) {
		// TODO
		//it->second->referenceCount++;
		//return it->second;
	}

	// TODO: Find the library file (if exists) and load the custom file format
	// return TryToLoadResource();

	return nullptr;
}

Resource::Type ResourceManager::GetTypeFromExtension(const char* extension) const
{
	Resource::Type ret = Resource::Type::MESH;

	return ret;
}

bool ResourceManager::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool ResourceManager::LoadConfiguration(Json& configModule)
{
	return true;
}

bool ResourceManager::InspectorDraw()
{
	return true;
}
