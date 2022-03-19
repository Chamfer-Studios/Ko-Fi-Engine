#include "ResourceManager.h"
#include "Engine.h"
#include "Log.h"
#include "RNG.h"
#include "FSDefs.h"
#include "Importer.h"

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
	if (assetPath == nullptr)
	{
		LOG_BOTH("Error loading file, file path was nullptr.");
		return -1;
	}
	std::string path = assetPath;
	std::string extension = path.substr(path.find_last_of(".") + 1);
	Resource::Type type = GetTypeFromExtension(extension.c_str());

	if (type == Resource::Type::UNKNOWN)
	{
		LOG_BOTH("Error loading file, unkown file type.");
		return -1;
	}

	std::string cleanPath = GetValidPath(assetPath);
	if (cleanPath.c_str() == nullptr)
	{
		LOG_BOTH("Error loading file, couldn't validate path.");
		return -1;
	}
	Resource* resource = CreateNewResource(cleanPath.c_str(), type);

	// TODO: Load from filesystem
	//char* fileBuffer = engine->filesystem->Load(assetPath);

	switch (resource->GetType())
	{
	case Resource::Type::MESH:
		//Importer::GetInstance()->meshImporter->Import(fileBuffer, (Mesh*)resource);
		break;
	case Resource::Type::TEXTURE:
		//Importer::GetInstance()->textureImporter->Import(fileBuffer, (Texture*)resource);
		break;
	case Resource::Type::SCENE:
		//Importer::GetInstance()->sceneImporter->Import(fileBuffer);
		break;
	case Resource::Type::SHADER:
		// TODO: Import Shader
		break;
	case Resource::Type::FONT:
		// TODO: Import Font
		break;
	case Resource::Type::UNKNOWN:
		break;
	default:
		break;
	}

	// TODO: Save the Resource
	SaveResource(resource);

	// Get the UID Resource to return
	UID ret = resource->GetUID();

	// TODO: Unload the resource after importing, we should only use the ID
	//RELEASE_ARRAY(fileBuffer);
	UnloadResource(resource);

	return ret;
}

Resource* ResourceManager::CreateNewResource(const char* assetPath, Resource::Type type)
{
	Resource* ret = new Resource(type);

	resourcesMap[ret->uid] = ret;
	ret->SetAssetsPathAndFile(assetPath,GetFileName(assetPath));
	ret->SetLibraryPathAndFile();
	return ret;
}

Resource* ResourceManager::RequestResource(UID uid)
{
	// Find if the resource is already loaded
	std::map<UID, Resource*>::iterator it = resourcesMap.find(uid);

	if (it != resourcesMap.end())
	{
		it->second->referenceCount++;
		return it->second;
	}

	// TODO: Find the library file (if exists) and load the custom file format
	std::map<UID, std::string>::iterator libIt = library.find(uid);
	if (libIt != library.end())
	{
		Resource* r = GetResourceFromLibrary(libIt->second.c_str());
		return r;
	}
	LOG_BOTH("FUCK YOU IT DOESNT EXIST");
	return nullptr;
}

UID ResourceManager::Find(const char* assetPath) const
{
	for (auto r : resourcesMap)
	{
		if (r.second->assetPath == assetPath)
			return r.first;
	}
	return -1;
}

void ResourceManager::SaveResource(Resource* resource)
{
	if (resource == nullptr)
	{
		LOG_BOTH("Error saving resource, resource was nullptr.");
		return;
	}

	switch (resource->type)
	{
	case Resource::Type::MESH:
		Importer::GetInstance()->meshImporter->Save((Mesh*)resource, resource->libraryPath.c_str());
		break;
	case Resource::Type::SCENE:
		Importer::GetInstance()->sceneImporter->Save((Scene*)resource);
		break;
	case Resource::Type::FONT:
		//TODO: Save Font
		break;
	default:
		break;
	}
}

void ResourceManager::UnloadResource(Resource* resource)
{
	UID uid = resource->uid;
	resource->CleanUp();
	RELEASE(resource);
	if (resourcesMap.find(uid) != resourcesMap.end())
		resourcesMap.erase(uid);
}

void ResourceManager::UnloadResource(UID uid)
{
	Resource* r = resourcesMap[uid];
	r->CleanUp();
	RELEASE(r);
	if (resourcesMap.find(uid) != resourcesMap.end())
		resourcesMap.erase(uid);
}

Resource* ResourceManager::GetResourceFromLibrary(const char* libraryPath)
{
	if (libraryPath == nullptr)
	{
		LOG_BOTH("Error getting resource, library path was nullptr.");
		return nullptr;
	}

	UID uid = LoadFromLibrary(libraryPath);
	return nullptr;
}

UID ResourceManager::LoadFromLibrary(const char* libraryPath)
{
	std::string cleanPath = GetValidPath(libraryPath);
	if (cleanPath.c_str() == nullptr)
	{
		LOG_BOTH("Error loading from library, couldn't validate path.");
		return -1;
	}

	//TODO: Wtf do i do now?
	return -1;
}

Resource::Type ResourceManager::GetTypeFromExtension(const char* extension) const
{
	Resource::Type ret = Resource::Type::UNKNOWN;

	//LUA?
	if (extension == "png" || extension == "PNG")
	{
		ret = Resource::Type::TEXTURE;
	}
	else if (extension == "fbx" || extension == "FBX")
	{
		ret = Resource::Type::MESH;
	}
	else if (extension == "json" || extension == "JSON")
	{
		ret = Resource::Type::SCENE;
	}
	else if (extension == "glsl" || extension == "GLSL")
	{
		ret = Resource::Type::SHADER;
	}
	else if (extension == "ttf" || extension == "TTF")
	{
		ret = Resource::Type::FONT;
	}

	return ret;
}

const char* ResourceManager::GetValidPath(const char* path) const
{
	std::string normalizedPath = path;

	for (uint i = 0; i < normalizedPath.size(); ++i)
	{
		if (normalizedPath[i] == '\\')
		{
			normalizedPath[i] = '/';
		}
	}

	size_t assetStart = normalizedPath.find("Assets");
	size_t libraryStart = normalizedPath.find("Library");
	std::string resultPath;
	if (assetStart != std::string::npos)
		resultPath = normalizedPath.substr(assetStart, normalizedPath.size());
	else if (libraryStart != std::string::npos)
		resultPath = normalizedPath.substr(libraryStart, normalizedPath.size());
	else
		LOG_BOTH("ERROR: Couldn't validate path.");

	return resultPath.c_str();
}

const char* ResourceManager::GetFileName(const char* path) const
{
	std::string p = path;
	std::string name = p.substr(p.find_last_of("/")+1, p.size());
	return name.c_str();
}