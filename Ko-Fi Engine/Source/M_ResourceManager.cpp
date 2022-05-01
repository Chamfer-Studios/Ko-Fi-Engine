#include "M_ResourceManager.h"
#include "Engine.h"
#include "Log.h"
#include "RNG.h"
#include "FSDefs.h"
#include "Importer.h"
#include "M_FileSystem.h"
#include "Resource.h"
#include "ResourceBase.h"

#include "R_Mesh.h"
#include "R_Texture.h"
#include "R_Material.h"
#include "R_Animation.h"
#include "R_Model.h"
#include "R_ParticleResource.h"
#include "R_Track.h"

#include "json.hpp"
#include "JsonHandler.h"
using Json = nlohmann::json;

#include <filesystem>

#include "optick.h"

M_ResourceManager::M_ResourceManager(KoFiEngine* engine) : Module(),
fileRefreshRate(0.0f),
fileRefreshTime(0.0f)
{
	name = "ResourceManager";
	this->engine = engine;
}

M_ResourceManager::~M_ResourceManager()
{

}

bool M_ResourceManager::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool M_ResourceManager::Start()
{
	CONSOLE_LOG("Starting M_ResourceManager...");
	appLog->AddLog("Starting M_ResourceManager...\n");

	fileRefreshRate = 5.0f;

	bool libCreated = engine->GetFileSystem()->CheckDirectory(LIBRARY_DIR);
	if (!libCreated)
	{
		CONSOLE_LOG("[FATAL ERROR] Resource Manager: Unable to create the Library directory.");
	}
	else
	{
		//RefreshDirectoryFiles(ASSETS_DIR);

		//TrimLibrary();

		//Find prefabs
	}

	return true;
}

bool M_ResourceManager::PreUpdate(float dt)
{
	OPTICK_EVENT();

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

bool M_ResourceManager::CleanUp()
{
	CONSOLE_LOG("Cleaning M_ResourceManager up...");
	appLog->AddLog("Cleaning M_ResourceManager up...\n");

	bool ret = true;

	for (std::map<UID, Resource*>::iterator it = resourcesMap.begin(); it != resourcesMap.end(); ++it)
	{
		it->second->CleanUp();
		RELEASE(it->second);
	}

	resourcesMap.clear();
	library.clear();

	return ret;
}

void M_ResourceManager::OnNotify(const Event& event)
{
	// Manage events
}

bool M_ResourceManager::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_ResourceManager::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_ResourceManager::InspectorDraw()
{
	return true;
}

UID M_ResourceManager::ImportFile(const char* assetPath)
{
	UID uid = 0;

	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: loading file, assets file path was nullptr.");
		return uid;
	}

	if (HasImportIgnoredExtension(assetPath))
	{
		CONSOLE_LOG("[WARNING] Resource Manager: loading file, the file extension has an import ignored extension: %s", assetPath);
		return uid;
	}

	std::string cleanPath = GetValidPath(assetPath);
	if (cleanPath.c_str() == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: loading file, couldn't validate path.");
		return uid;
	}

	bool metaIsValid = ValidateMetaFile(assetPath);
	if (metaIsValid)
	{
		CONSOLE_LOG("[STATUS] Resource Manager: file to import was already in the library.");

		std::map<UID, ResourceBase> libraryItems;
		GetLibraryPairs(assetPath, libraryItems);

		for (const auto& item : libraryItems)
		{
			if (library.find(item.first) == library.end())
				library.emplace(std::make_pair(item.first, item.second));
		}
		uid = libraryItems.begin()->first;
		libraryItems.clear();
	}
	else
	{
		if (HasMetaFile(assetPath))
			DeleteFromLibrary(assetPath);

		uid = ImportFromAssets(assetPath);

		if (uid == 0)
			CONSOLE_LOG("[ERROR] Resource Manager: loading file, error loading file from assets.");
	}

	return uid;
}

bool M_ResourceManager::SaveResource(Resource* resource)
{
	bool ret = true;

	if (resource == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: saving resource, resource was nullptr.");
		return false;
	}

	switch (resource->GetType())
	{
	case ResourceType::MESH:
		ret = Importer::GetInstance()->meshImporter->Save((R_Mesh*)resource, resource->GetLibraryPath());
		break;
	case ResourceType::TEXTURE:
		ret = Importer::GetInstance()->textureImporter->Save((R_Texture*)resource, resource->GetLibraryPath());
		break;
	case ResourceType::MODEL:
		ret = Importer::GetInstance()->sceneImporter->SaveModel((R_Model*)resource, resource->GetLibraryPath());
		break;
	case ResourceType::MATERIAL:
		ret = Importer::GetInstance()->materialImporter->Save((R_Material*)resource, resource->GetLibraryPath());
		break;
	case ResourceType::ANIMATION:
		ret = Importer::GetInstance()->animationImporter->Save((R_Animation*)resource, resource->GetLibraryPath());
		break;
		//case ResourceType::SCENE:
			// TODO ret = Importer::GetInstance()->sceneImporter->Save((Scene*)resource);
			//break;
		//case ResourceType::TRACK:
			// TODO
			//break;
		//case ResourceType::PARTICLE:
			// TODO
			//break;
	case ResourceType::UNKNOWN:
		return false;
		break;
	default:
		break;
	}

	if (ResourceHasMetaType(resource))
	{
		SaveMetaFile(resource);
	}

	library.emplace(resource->GetUID(), ResourceBase(resource));

	return ret;
}

Resource* M_ResourceManager::GetResourceFromLibrary(const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting resource, library path was nullptr.");
		return nullptr;
	}

	std::string metaPath = assetPath + std::string(META_EXTENSION);
	
	std::string errorString = "[ERROR] Resource Manager: couldn't get resource from { " + metaPath + " }";

	if (!std::filesystem::exists(metaPath))
	{
		CONSOLE_LOG("%s Couldn't find meta file.", errorString.c_str());
		return nullptr;
	}

	UID uid = LoadFromLibrary(assetPath);
	if (uid == 0)
	{
		CONSOLE_LOG("%s Couldn't get resource uid from assets path.", errorString.c_str());
		return nullptr;
	}

	Resource* resource = RequestResource(uid);
	if (resource == nullptr)
		CONSOLE_LOG("%s Couldn't request resource.", errorString.c_str());

	return resource;
}

UID M_ResourceManager::LoadFromLibrary(const char* assetPath)
{
	std::string errorString = "[ERROR] Resource Manager: couldn't load file from library path.";

	std::string cleanPath = GetValidPath(assetPath);
	if (cleanPath.c_str() == nullptr)
	{
		CONSOLE_LOG("%s Path was nullptr.", errorString.c_str());
		return 0;
	}

	std::string metaPath = cleanPath + META_EXTENSION;

	if (!std::filesystem::exists(metaPath))
	{
		CONSOLE_LOG("%s Couldn't find meta file.", errorString.c_str());
		return 0;
	}

	JsonHandler jsonHandler;
	Json jsonMeta;

	bool jsonLoaded = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	bool metaIsValid = ValidateMetaFile(jsonMeta);
	
	if (jsonMeta.empty())
	{
		CONSOLE_LOG("%s Couldn't get the json meta node.", errorString.c_str());
		return 0;
	}

	if (!metaIsValid)
	{
		CONSOLE_LOG("%s Couldn't validate json meta node.", errorString.c_str());
		return 0;
	}

	UID uid = 0;
	if (jsonLoaded && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		uid = jsonMeta.at("uid");
		if (resourcesMap.find(uid) != resourcesMap.end())
			return uid;

		bool loadedResource = LoadResource(uid, assetPath);
		if (!loadedResource)
		{
			CONSOLE_LOG("%s Couldn't load resource in memory.", errorString.c_str());
			return 0;
		}

		std::string containedPath = "";
		if (jsonMeta.contains("contained_resources"))
		{
			if (!jsonMeta.at("contained_resources").is_null() && !jsonMeta.at("contained_resources").empty())
			{
				for (const auto& containedIt : jsonMeta.at("contained_resources").items())
				{
					UID containedUid = containedIt.value().at("uid");
					if (resourcesMap.find(containedUid) != resourcesMap.end())
						continue;

					std::filesystem::path tmpPath = assetPath;
					containedPath = containedIt.value().at("asset_file");
					containedPath = tmpPath.parent_path().string() + "/" + containedPath;

					bool loadedResource = LoadResource(containedUid, containedPath.c_str());
					if (!loadedResource)
					{
						CONSOLE_LOG("%s Couldn't load resource in memory.", errorString.c_str());
						return 0;
					}

					containedPath.clear();
					containedPath.shrink_to_fit();
					tmpPath.clear();
				}
			}
		}
	}
	else
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting forced UIDs from meta. Asset path had no associated meta file.");
	}

	return uid;
}

Resource* M_ResourceManager::CreateNewResource(const ResourceType& type, const char* assetPath, UID forcedUid)
{
	Resource* resource = nullptr;

	switch (type)
	{
	case ResourceType::MESH: { resource = new R_Mesh(); } break;
	case ResourceType::TEXTURE: { resource = new R_Texture(); } break;
	case ResourceType::MODEL: { resource = new R_Model(); } break;
	case ResourceType::MATERIAL: { resource = new R_Material(); } break;
	case ResourceType::ANIMATION: { resource = new R_Animation(); } break;
		//case ResourceType::SCENE: { resource = new R_Scene(); } break;
		//case ResourceType::FONT: { resource = new R_Font(); } break;
		//case ResourceType::TRACK: { resource = new R_Track(); } break;
		//case ResourceType::PARTICLE: { resource = new R_Particle(); } break;
	case ResourceType::UNKNOWN: { resource = nullptr; } break;
	default:
		break;
	}

	if (resource != nullptr)
	{
		if (assetPath != nullptr)
			resource->SetAssetsPathAndFile(assetPath, engine->GetFileSystem()->GetFileName(assetPath).c_str());

		if (forcedUid != 0)
			resource->ForceUID(forcedUid);
		else
			resource->SetLibraryPathAndFile();
	}

	return resource;
}

Resource* M_ResourceManager::RequestResource(UID uid)
{
	if (uid == 0)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: resource request failed! Requested UID was 0.");
		return nullptr;
	}

	std::map<UID, Resource*>::iterator it = resourcesMap.find(uid);
	if (it != resourcesMap.end())
	{
		it->second->ModifyReferenceCount(1);
		return it->second;
	}
	return nullptr;
}

bool M_ResourceManager::LoadResource(UID uid, const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: could not load resource, assets path was nullptr.");
		return false;
	}
	if (uid == 0)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: could not load resource, invalid uid.");
		return false;
	}
	if (library.find(uid) == library.end())
	{
		CONSOLE_LOG("[ERROR] Resource Manager: could not load resource, resource could not be found in library.");
		return false;
	}

	std::map<UID, Resource*>::iterator item = resourcesMap.find(uid);
	if (item != resourcesMap.end())
	{
		return true;
	}

	std::string libraryPath = library.find(uid)->second.libraryPath;

	ResourceType type = GetTypeFromPathExtension(libraryPath.c_str());
	Resource* resource = CreateNewResource(type, assetPath, uid);

	bool ret = false;
	switch (type)
	{
	case ResourceType::MESH:
		ret = Importer::GetInstance()->meshImporter->Load(libraryPath.c_str(), (R_Mesh*)resource);
		break;
	case ResourceType::TEXTURE:
		ret = Importer::GetInstance()->textureImporter->Load(libraryPath.c_str(), (R_Texture*)resource);
		break;
	case ResourceType::MODEL:
		ret = Importer::GetInstance()->sceneImporter->LoadModel(libraryPath.c_str(), (R_Model*)resource);
		break;
	case ResourceType::MATERIAL:
		ret = Importer::GetInstance()->materialImporter->Load(libraryPath.c_str(), (R_Material*)resource);
		break;
	case ResourceType::ANIMATION:
		ret = Importer::GetInstance()->animationImporter->Load(libraryPath.c_str(), (R_Animation*)resource);
		break;
		//case ResourceType::PARTICLE:
		// TODO
		//	break;
	case ResourceType::UNKNOWN:
		return ret;
		break;
	default:
		break;
	}

	if (ret)
	{
		resourcesMap.emplace(resource->GetUID(), resource);
		CONSOLE_LOG("[STATUS] Resource Manager: successfully loaded resource: %s in memory.", resource->GetAssetFile());
	}
	else
	{
		UnloadResource(resource);
		CONSOLE_LOG("[ERROR] Resource Manager: couldn't load the resource data from file: %s.", libraryPath.c_str());
	}

	return ret;
}

bool M_ResourceManager::FreeResource(UID uid)
{
	if (uid == 0)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: resource free failed! Requested UID was 0.");
		return false;
	}

	std::map<UID, Resource*>::iterator it = resourcesMap.find(uid);
	if (it == resourcesMap.end())
	{
		CONSOLE_LOG("[ERROR] Resource Manager: resource free failed! Requested UID wasn't in the resources map.");
		return false;
	}

	it->second->ModifyReferenceCount(-1);

	if (it->second->GetReferenceCount() == 0)
		UnloadResource(uid);
}

bool M_ResourceManager::UnloadResource(Resource* resource)
{
	if (resource == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: trying to unload resource, resource was nullptr.");
		return false;
	}

	UID uid = resource->GetUID();

	resource->CleanUp();
	RELEASE(resource);

	if (resourcesMap.find(uid) != resourcesMap.end())
		resourcesMap.erase(uid);
	else
	{
		CONSOLE_LOG("[WARNING] Resource Manager: trying to unload resource, unloaded resource was not inside map!");
		return false;
	}

	return true;
}

bool M_ResourceManager::UnloadResource(UID uid)
{
	if (uid == 0)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: Could not Deallocate Resource! Error: Given UID was 0");
		return false;
	}

	std::map<UID, Resource*>::iterator it = resourcesMap.find(uid);
	if (it == resourcesMap.end())
	{
		CONSOLE_LOG("[ERROR] Resource Manager: couldn't unload resource. Resource was not allocated in memory.");
		return false;
	}

	it->second->CleanUp();
	RELEASE(it->second);
	resourcesMap.erase(uid);

	return true;
}

bool M_ResourceManager::DeleteAndUnloadResource(UID uid)
{
	if (library.find(uid) != library.end())
		library.erase(uid);

	std::map<UID, Resource*>::iterator it = resourcesMap.find(uid);
	if (it == resourcesMap.end())
	{
		CONSOLE_LOG("[ERROR] Resource Manager: trying to delete resource, resource was not inside map!");
		return false;
	}

	if (it->second != nullptr)
	{
		it->second->CleanUp();
		RELEASE(it->second);
	}

	resourcesMap.erase(uid);

	return true;
}

Resource* M_ResourceManager::Find(const char* assetPath) const
{
	for (const auto& r : resourcesMap)
	{
		if (r.second->GetAssetPath() == assetPath)
			return r.second;
	}
	return nullptr;
}

void M_ResourceManager::FindAndForceUID(Resource* resource)
{
	if (resource == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: Resource pointer is nullptr.");
		return;
	}

	UID forcedUID = GetForcedUIDFromMeta(resource->GetAssetPath());
	if (forcedUID != 0)
		resource->ForceUID(forcedUID);
}

UID M_ResourceManager::GetForcedUIDFromMeta(const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: Could not get forced UID from meta, asset path was nullptr.");
		return 0;
	}

	std::string metaPath = assetPath + std::string(META_EXTENSION);

	if (!std::filesystem::exists(metaPath))
		return 0;

	JsonHandler jsonHandler;
	Json jsonMeta;

	bool success = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	UID uid = 0;
	if (success && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		uid = jsonMeta.at("uid");
		if (uid == 0)
			CONSOLE_LOG("[ERROR] Resource Manager: Resource UID was 0.");
	}
	else
		CONSOLE_LOG("[ERROR] Resource Manager: getting forced UID from meta. Asset path had no associated meta file.");

	return uid;
}

bool M_ResourceManager::GetForcedUIDsFromMeta(const char* assetPath, std::map<std::string, UID>& uids)
{
	bool ret = true;

	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting forced UIDs from meta, asset path was nullptr.");
		return false;
	}

	std::string metaPath = assetPath + std::string(META_EXTENSION);

	if (!std::filesystem::exists(metaPath))
		return false;

	JsonHandler jsonHandler;
	Json jsonMeta;

	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		std::string name = jsonMeta.at("asset_file");
		UID uid = jsonMeta.at("uid");
		if (uid == 0)
		{
			CONSOLE_LOG("[ERROR] Resource Manager: main resource UID was 0.");
			return false;
		}

		uids.emplace(name, uid);

		if (jsonMeta.contains("contained_resources"))
		{
			if (!jsonMeta.at("contained_resources").is_null() && !jsonMeta.at("contained_resources").empty())
			{
				for (const auto& containedIt : jsonMeta.at("contained_resources").items())
				{
					std::string containedName = containedIt.value().at("asset_file");
					UID containedUid = containedIt.value().at("uid");
					if (containedUid == 0)
					{
						CONSOLE_LOG("[ERROR] Resource Manager: contained resource UID was 0.");
						continue;
					}
					uids.emplace(containedName, containedUid);
				}
			}
		}
	}
	else
		CONSOLE_LOG("[ERROR] Resource Manager: getting forced UIDs from meta. Asset path had no associated meta file.");

	return ret;
}

bool M_ResourceManager::ImportMaterial(const char* assetPath, R_Material* material)
{
	FindAndForceUID(material);
	bool ret = Importer::GetInstance()->materialImporter->Load(assetPath, material);
	return (ret && material != nullptr);
}

bool M_ResourceManager::ImportTexture(const char* assetPath, R_Texture* texture)
{
	FindAndForceUID(texture);
	bool ret = Importer::GetInstance()->textureImporter->Import(assetPath, (R_Texture*)texture);
	return (ret && texture != nullptr);
}

void M_ResourceManager::RefreshDirectoryFiles(const char* directory)
{
	if (directory == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: trying to refresh directory files, string was nullptr.");
		return;
	}

	std::vector<std::string> toImport;
	std::vector<std::string> toUpdate;
	std::vector<std::string> toDelete;

	std::vector<std::string> assetsFiles;
	std::vector<std::string> metaFiles;
	std::map<std::string, std::string> filePairs;

	engine->GetFileSystem()->DiscoverAllFilesFiltered(directory, assetsFiles, metaFiles, META_EXTENSION);

	FindFilesToImport(assetsFiles, metaFiles, filePairs, toImport);
	FindFilesToUpdate(filePairs, toUpdate);
	FindFilesToDelete(metaFiles, filePairs, toDelete);

	LoadFilesIntoLibrary(filePairs);

	filePairs.clear();
	metaFiles.clear();
	metaFiles.shrink_to_fit();
	assetsFiles.clear();
	assetsFiles.shrink_to_fit();

	for (uint i = 0; i < toDelete.size(); ++i)
		DeleteFromLibrary(toDelete[i].c_str());

	for (uint i = 0; i < toUpdate.size(); ++i)
	{
		std::filesystem::path pathUpdate = toUpdate[i].c_str();
		std::string extension = pathUpdate.extension().string();

		if (engine->GetFileSystem()->StringCompare(extension.c_str(), FBX_EXTENSION) == 0 ||
			engine->GetFileSystem()->StringCompare(extension.c_str(), SHADER_EXTENSION) == 0 ||
			engine->GetFileSystem()->StringCompare(extension.c_str(), PNG_EXTENSION) == 0)
		{
			DeleteFromLibrary(toUpdate[i].c_str());
			ImportFile(toUpdate[i].c_str());
		}
	}

	for (uint i = 0; i < toImport.size(); ++i)
		ImportFile(toImport[i].c_str());

	toDelete.clear();
	toDelete.shrink_to_fit();
	toUpdate.clear();
	toUpdate.shrink_to_fit();
	toImport.clear();
	toImport.shrink_to_fit();
}

void M_ResourceManager::FindFilesToImport(std::vector<std::string>& assetsFiles, std::vector<std::string>& metaFiles, std::map<std::string, std::string>& filePairs, std::vector<std::string>& toImport)
{
	if (assetsFiles.empty())
		return;

	std::string metaFile = "";
	for (uint i = 0; i < assetsFiles.size(); ++i)
	{
		if (HasImportIgnoredExtension(assetsFiles[i].c_str()))
			continue;

		metaFile = assetsFiles[i] + META_EXTENSION;
		bool find = false;
		for (uint j = 0; j < metaFiles.size(); ++j)
		{
			if (metaFile == metaFiles[j])
				find = true;
		}

		if (find)
		{
			filePairs.emplace(assetsFiles[i], metaFile);

			if (!ValidateMetaFile(assetsFiles[i].c_str(), false))
				toImport.push_back(assetsFiles[i]);
		}
		else
			toImport.push_back(assetsFiles[i]);
	}

	metaFile.clear();
	metaFile.shrink_to_fit();
}

void M_ResourceManager::FindFilesToUpdate(std::map<std::string, std::string>& filePairs, std::vector<std::string>& toUpdate)
{
	if (filePairs.empty())
		return;

	for (const auto& item : filePairs)
	{
		int assetTime = engine->GetFileSystem()->GetLastModTime(item.first.c_str());
		int metaTime = GetModTimeFromMeta(item.second.c_str());

		if (assetTime != metaTime)
		{
			CONSOLE_LOG("[STATUS] Resource Manager: modification time discrepancy with file %s\n", item.first.c_str());
			toUpdate.push_back(item.first);
		}
	}
}

void M_ResourceManager::FindFilesToDelete(std::vector<std::string>& metaFiles, std::map<std::string, std::string>& filePairs, std::vector<std::string>& toDelete)
{
	if (metaFiles.empty())
		return;

	std::string assetPath = "";
	for (uint i = 0; i < metaFiles.size(); ++i)
	{
		assetPath = metaFiles[i].substr(0, metaFiles[i].find_last_of("."));
		if (filePairs.find(assetPath) == filePairs.end())
			toDelete.push_back(assetPath);
	}
}

void M_ResourceManager::LoadFilesIntoLibrary(std::map<std::string, std::string>& filePairs)
{
	if (filePairs.empty())
		return;

	for (const auto& item : filePairs)
		LoadMetaLibraryPairsIntoLibrary(item.first.c_str());
}

void M_ResourceManager::DeleteFromLibrary(const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: deleting file from library, asset path was nullptr.");
		return;
	}

	if (!HasMetaFile(assetPath))
	{
		CONSOLE_LOG("[ERROR] Resource Manager: deleting file from library, %s file couldn't be found or doesn't exist.", assetPath);
		return;
	}

	std::vector<UID> resourceUids;
	std::vector<std::string> toDelete;

	GetResourceUIDsFromMeta(assetPath, resourceUids);
	GetLibraryFilePathsFromMeta(assetPath, toDelete);

	for (uint i = 0; i < resourceUids.size(); ++i)
		DeleteAndUnloadResource(resourceUids[i]);

	for (uint i = 0; i < toDelete.size(); ++i)
	{
		std::filesystem::remove(toDelete[i].c_str());
		std::filesystem::remove(std::string(assetPath) + META_EXTENSION);
	}

	toDelete.clear();
	toDelete.shrink_to_fit();
	resourceUids.clear();
	resourceUids.shrink_to_fit();
}

void M_ResourceManager::DeleteFromAssets(const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: deleting from assets, asset path was nullptr.");
		return;
	}

	std::vector<UID> resourceUIDs;
	std::vector<std::string> toDelete;

	GetResourceUIDsFromMeta(assetPath, resourceUIDs);
	GetLibraryFilePathsFromMeta(assetPath, toDelete);

	std::string metaPath = assetPath + std::string(META_EXTENSION);
	toDelete.push_back(assetPath);
	toDelete.push_back(metaPath);

	for (uint i = 0; i < resourceUIDs.size(); ++i)
		DeleteAndUnloadResource(resourceUIDs[i]);

	for (uint i = 0; i < toDelete.size(); ++i)
		std::filesystem::remove(toDelete[i].c_str());

	toDelete.clear();
	toDelete.shrink_to_fit();
	resourceUIDs.clear();
	resourceUIDs.shrink_to_fit();
}

bool M_ResourceManager::TrimLibrary()
{
	std::vector<std::string> files;
	engine->GetFileSystem()->DiscoverAllFiles(LIBRARY_DIR, files);

	std::map<std::string, UID> fileUIDs;
	for (auto file = files.cbegin(); file != files.cend(); ++file)
	{
		UID uid = 0;
		sscanf(engine->GetFileSystem()->GetFileName((*file).c_str()).c_str(), "%u", &uid);

		if (uid != 0)
			fileUIDs.emplace((*file), uid);
	}

	for (std::map<std::string, UID>::const_iterator UID = fileUIDs.cbegin(); UID != fileUIDs.cend(); ++UID)
	{
		if (library.find(UID->second) == library.end())
			std::filesystem::remove(UID->first.c_str());
	}

	fileUIDs.clear();

	files.clear();
	files.shrink_to_fit();

	return true;
}

bool M_ResourceManager::GetResourceUIDsFromMeta(const char* assetPath, std::vector<UID>& uids)
{
	bool ret = true;

	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting UIDs from meta, asset path was nullptr.");
		return false;
	}

	JsonHandler jsonHandler;
	Json jsonMeta;

	std::string metaPath = assetPath + std::string(META_EXTENSION);
	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		UID uid = jsonMeta.at("uid");
		if (uid == 0)
		{
			CONSOLE_LOG("[ERROR] Resource Manager: main resource UID was 0.");
			return false;
		}

		uids.push_back(uid);

		if (jsonMeta.contains("contained_resources"))
		{
			if (!jsonMeta.at("contained_resources").is_null() && !jsonMeta.at("contained_resources").empty())
			{
				for (const auto& containedIt : jsonMeta.at("contained_resources").items())
				{
					UID containedUid = containedIt.value().at("uid");
					if (containedUid == 0)
					{
						CONSOLE_LOG("[ERROR] Resource Manager: contained resource UID was 0.");
						continue;
					}
					uids.push_back(containedUid);
				}
			}
		}
	}
	else
		CONSOLE_LOG("[ERROR] Resource Manager: getting UIDs from meta. Asset path had no associated meta file.");

	return ret;
}

bool M_ResourceManager::GetResourceBasesFromMeta(const char* assetPath, std::vector<ResourceBase>& bases)
{
	bool ret = true;

	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting resource bases from meta, asset path was nullptr.");
		return false;
	}

	std::string errorString = "[ERROR] Resource Manager: could not get resource base file paths from { " + std::string(assetPath) + " }'s meta file";

	JsonHandler jsonHandler;
	Json jsonMeta;

	std::string metaPath = assetPath + std::string(META_EXTENSION);
	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		UID uid = jsonMeta.at("uid");
		ResourceType type = (ResourceType)jsonMeta.at("type").get<int>();
		std::string assetPath = jsonMeta.at("asset_path");
		std::string assetFile = jsonMeta.at("asset_file");
		std::string libraryPath = jsonMeta.at("library_path");
		std::string libraryFile = jsonMeta.at("library_file");

		if (uid == 0)
		{
			CONSOLE_LOG("[ERROR] Resource Manager: main resource UID was 0.");
			return false;
		}
		bases.push_back(ResourceBase(uid, type, assetPath, assetFile, libraryPath, libraryFile));

		if (jsonMeta.contains("contained_resources"))
		{
			if (!jsonMeta.at("contained_resources").is_null() && !jsonMeta.at("contained_resources").empty())
			{
				for (const auto& containedIt : jsonMeta.at("contained_resources").items())
				{
					UID containedUid = containedIt.value().at("uid");
					ResourceType containedType = (ResourceType)containedIt.value().at("type").get<int>();

					std::string containedAssetPath = "";
					std::string containedAssetFile = containedIt.value().at("asset_file");

					std::string containedLibraryPath = containedIt.value().at("library_path");
					std::string containedLibraryFile = "";

					std::string directory = "";
					bool success = GetAssetDirectoryFromType(containedType, directory);
					if (!success)
						continue;

					if (containedUid == 0)
					{
						CONSOLE_LOG("[ERROR] Resource Manager: contained resource UID was 0.");
						continue;
					}

					containedAssetPath = directory + containedAssetFile;
					containedLibraryFile = engine->GetFileSystem()->GetFileName(containedLibraryPath.c_str());
					bases.push_back(ResourceBase(containedUid, containedType, containedAssetPath, containedAssetFile, containedLibraryPath, containedLibraryFile));
				}
			}
		}
	}
	else
		CONSOLE_LOG("%s Asset path had no associated meta file.", errorString.c_str());

	return ret;
}

bool M_ResourceManager::GetLibraryFilePathsFromMeta(const char* assetPath, std::vector<std::string>& paths)
{
	bool ret = true;

	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting library file paths from meta, asset path was nullptr.");
		return false;
	}

	std::string errorString = "[ERROR] Resource Manager: could not get library file paths from { " + std::string(assetPath) + " }'s meta file";

	JsonHandler jsonHandler;
	Json jsonMeta;

	std::string metaPath = assetPath + std::string(META_EXTENSION);
	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		UID uid = jsonMeta.at("uid");
		ResourceType type = (ResourceType)jsonMeta.at("type").get<int>();

		std::string directory = "";
		std::string extension = "";
		bool success = GetLibraryDirectoryAndExtensionFromType(type, directory, extension);
		if (!success)
			return false;

		if (uid == 0)
		{
			CONSOLE_LOG("%s Main resource UID was 0.", errorString.c_str());
			return false;
		}
		if (directory == "" || extension == "")
			return false;

		std::string libraryPath = directory + std::to_string(uid) + extension;
		paths.push_back(libraryPath);

		if (jsonMeta.contains("contained_resources"))
		{
			if (!jsonMeta.at("contained_resources").is_null() && !jsonMeta.at("contained_resources").empty())
			{
				std::string containedPath = "";
				for (const auto& containedIt : jsonMeta.at("contained_resources").items())
				{
					UID containedUid = containedIt.value().at("uid");
					ResourceType containedType = (ResourceType)containedIt.value().at("type").get<int>();

					directory = "";
					extension = "";
					success = GetLibraryDirectoryAndExtensionFromType(containedType, directory, extension);
					if (!success || containedUid == 0 || directory == "" || extension == "")
						continue;

					containedPath = directory + std::to_string(containedUid) + extension;
					paths.push_back(containedPath);
				}
			}
		}
	}
	else
		CONSOLE_LOG("%s Asset path had no associated meta file.", errorString.c_str());

	return ret;
}

bool M_ResourceManager::GetAssetDirectoryFromType(const ResourceType& type, std::string& directory)
{
	switch (type)
	{
	case ResourceType::MODEL: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::MESH: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::ANIMATION: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::TEXTURE: { directory = ASSETS_TEXTURES_DIR; } break;
	case ResourceType::MATERIAL: { directory = ASSETS_SHADERS_DIR; } break;
		//case ResourceType::SCENE: { directory = ASSETS_SCENES_DIR; } break;
		//case ResourceType::TRACK: { directory = ASSETS_AUDIO_DIR; } break;
		//case ResourceType::FONT: { directory = ASSETS_FONTS_DIR; } break;
		//case ResourceType::PARTICLE: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::UNKNOWN:
	{
		CONSOLE_LOG("[ERROR] Resource Manager: couldn't return asset directory from type because it is UNKNOWN.");
		return false;
	}
	break;
	default: break;
	}

	return true;
}

bool M_ResourceManager::GetLibraryDirectoryAndExtensionFromType(const ResourceType& type, std::string& directory, std::string& extension)
{
	switch (type)
	{
	case ResourceType::MODEL: { directory = MODELS_DIR; extension = MODEL_EXTENSION; } break;
	case ResourceType::MESH: { directory = MESHES_DIR; extension = MESH_EXTENSION; } break;
	case ResourceType::ANIMATION: { directory = ANIMATIONS_DIR; extension = ANIMATION_EXTENSION; } break;
	case ResourceType::TEXTURE: { directory = TEXTURES_DIR; extension = TEXTURE_EXTENSION; } break;
	case ResourceType::MATERIAL: { directory = SHADERS_DIR;  extension = SHADER_EXTENSION; } break;
		//case ResourceType::SCENE: { directory = SCENES_DIR; extension = SCENE_EXTENSION; } break;
		//case ResourceType::TRACK: { directory = AUDIOS_DIR; extension = MODEL_EXTENSION; } break;
		//case ResourceType::FONT: { directory = FONTS_DIR; extension = FONT_EXTENSION; } break;
		//case ResourceType::PARTICLE: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::UNKNOWN:
	{
		CONSOLE_LOG("[ERROR] Resource Manager: couldn't return library directory from type because it is UNKNOWN.");
		return false;
	}
	break;
	default:
		break;
	}

	return true;
}

bool M_ResourceManager::LoadMetaLibraryPairsIntoLibrary(const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: loading meta file into library, asset path was nullptr.");
		return false;
	}

	std::map<UID, ResourceBase> libraryPairs;
	GetLibraryPairs(assetPath, libraryPairs);

	if (libraryPairs.empty())
		return true;

	for (auto item : libraryPairs)
		library.emplace(item.first, item.second);

	libraryPairs.clear();

	return true;
}

bool M_ResourceManager::GetLibraryPairs(const char* assetPath, std::map<UID, ResourceBase>& pairs)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting library pairs, asset path was nullptr.");
		return false;
	}

	std::string errorString = "[ERROR] Resource Manager: could not get library file pairs from { " + std::string(assetPath) + " }'s meta file.";

	std::vector<UID> resourceUIDs;
	std::vector<ResourceBase> bases;
	GetResourceUIDsFromMeta(assetPath, resourceUIDs);
	GetResourceBasesFromMeta(assetPath, bases);

	if (resourceUIDs.size() != bases.size())
		CONSOLE_LOG("%s Missmatching resource bases and UIDs.", errorString.c_str());

	for (uint i = 0; i < resourceUIDs.size(); ++i)
		pairs.emplace(resourceUIDs[i], bases[i]);

	resourceUIDs.clear();
	resourceUIDs.shrink_to_fit();
	bases.clear();
	bases.shrink_to_fit();

	return true;
}

int M_ResourceManager::GetModTimeFromMeta(const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting modification time from meta, asset path was nullptr.");
		return 0;
	}

	std::string errorString = "[ERROR] Resource Manager: could not get modification time from { " + std::string(assetPath) + " }'s meta file";

	JsonHandler jsonHandler;
	Json jsonMeta;

	std::string metaPath = assetPath;
	bool loaded = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (loaded && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		int modTime = jsonMeta.at("mod_time");
		return modTime;
	}
	else
	{
		CONSOLE_LOG("%s Asset path had no associated meta file.", errorString.c_str());
		return 0;
	}

	return 0;
}

UID M_ResourceManager::ImportFromAssets(const char* assetPath)
{
	UID uid = 0;

	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: loading from assets, path was nullptr.");
		return uid;
	}

	ResourceType type = GetTypeFromPathExtension(assetPath);
	Resource* resource = CreateNewResource(type, assetPath);

	bool success = false;
	switch (type)
	{
	case ResourceType::TEXTURE:
		success = ImportTexture(assetPath, (R_Texture*)resource);
		break;
	case ResourceType::MODEL:
		success = Importer::GetInstance()->sceneImporter->Import((R_Model*)resource);
		break;
	case ResourceType::MATERIAL:
		success = ImportMaterial(assetPath, (R_Material*)resource);
		break;
		//case ResourceType::SCENE:
			//TODO: SCENE IMPORT
			//break;
		//case ResourceType::FONT:
			//TODO: FONT IMPORT
			//break;
	default:
		break;
	}

	if (!success)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: could not import asset file: %s", assetPath);
		UnloadResource(resource);
		return 0;
	}

	uid = resource->GetUID();

	SaveResource(resource);

	UnloadResource(resource);

	return uid;
}

std::string M_ResourceManager::GetValidPath(const char* path) const
{
	std::string normalizedPath = path;

	for (uint i = 0; i < normalizedPath.size(); ++i)
	{
		if (normalizedPath[i] == '\\')
			normalizedPath[i] = '/';
	}

	size_t assetStart = normalizedPath.find("Assets");
	size_t libraryStart = normalizedPath.find("Library");

	std::string resultPath;

	if (assetStart != std::string::npos)
		resultPath = normalizedPath.substr(assetStart, normalizedPath.size());
	else if (libraryStart != std::string::npos)
		resultPath = normalizedPath.substr(libraryStart, normalizedPath.size());
	else
		CONSOLE_LOG("[ERROR] Resource Manager: Couldn't validate path.");

	return resultPath;
}

ResourceType M_ResourceManager::GetTypeFromPathExtension(const char* path)
{
	ResourceType ret = ResourceType::UNKNOWN;

	std::filesystem::path newPath = path;
	std::string extension = newPath.extension().string();

	if (engine->GetFileSystem()->StringCompare(extension.c_str(), PNG_EXTENSION) == 0)
		ret = ResourceType::TEXTURE;
	else if (engine->GetFileSystem()->StringCompare(extension.c_str(), TEXTURE_EXTENSION) == 0)
		ret = ResourceType::TEXTURE;
	else if (engine->GetFileSystem()->StringCompare(extension.c_str(), FBX_EXTENSION) == 0)
		ret = ResourceType::MODEL;
	else if (engine->GetFileSystem()->StringCompare(extension.c_str(), MODEL_EXTENSION) == 0)
		ret = ResourceType::MODEL;
	else if (engine->GetFileSystem()->StringCompare(extension.c_str(), MESH_EXTENSION) == 0)
		ret = ResourceType::MESH;
	else if (engine->GetFileSystem()->StringCompare(extension.c_str(), ANIMATION_EXTENSION) == 0)
		ret = ResourceType::ANIMATION;
	else if (engine->GetFileSystem()->StringCompare(extension.c_str(), SHADER_EXTENSION) == 0)
		ret = ResourceType::MATERIAL;
	//else if (engine->GetFileSystem()->StringCompare(extension.c_str(), SCENE_EXTENSION) == 0)
		//ret = ResourceType::SCENE;
	//else if (engine->GetFileSystem()->StringCompare(extension.c_str(), FONT_EXTENSION) == 0)
		//ret = ResourceType::FONT;
	else
		CONSOLE_LOG("[ERROR] Resource Manager: couldn't import from the given asset path. File extension: %s is not supported.", extension.c_str());

	return ret;
}

bool M_ResourceManager::SaveMetaFile(Resource* resource) const
{
	bool ret = true;

	if (resource == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: saving meta file, resource was nullptr.");
		return false;
	}

	Json jsonMeta;
	jsonMeta["uid"] = resource->GetUID();
	jsonMeta["type"] = (int)resource->GetType();
	jsonMeta["asset_file"] = resource->GetAssetFile();
	jsonMeta["asset_path"] = resource->GetAssetPath();
	jsonMeta["library_file"] = resource->GetLibraryFile();
	jsonMeta["library_path"] = resource->GetLibraryPath();
	jsonMeta["mod_time"] = engine->GetFileSystem()->GetLastModTime(resource->GetAssetPath());

	resource->SaveMeta(jsonMeta);

	std::string path = resource->GetAssetPath() + std::string(META_EXTENSION);

	JsonHandler jsonHandler;
	ret = jsonHandler.SaveJson(jsonMeta, path.c_str());

	if (ret)
		CONSOLE_LOG("[STATUS] Resource Manager: meta file saved for resource: %s", resource->GetAssetPath());
	else
		CONSOLE_LOG("[ERROR] Resource Manager: Couldn't save meta file for resource: %s", resource->GetAssetPath());

	return ret;
}

bool M_ResourceManager::HasMetaFile(const char* assetPath)
{
	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: checking for meta file, asset path was nullptr.");
		return false;
	}

	std::string path = assetPath + std::string(META_EXTENSION);
	return std::filesystem::exists(path);
}

bool M_ResourceManager::ValidateMetaFile(const char* assetPath, bool libraryCheck)
{
	bool ret = true;

	if (assetPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: validating meta file, assetPath was nullptr.");
		return false;
	}

	std::string metaPath = assetPath + std::string(META_EXTENSION);

	std::string errorString = "[ERROR] Resource Manager: validating meta file: " + metaPath;

	if (!std::filesystem::exists(metaPath))
	{
		CONSOLE_LOG("[STATUS] Resource Manager: couldn't find meta file.");
		return false;
	}

	JsonHandler jsonHandler;
	Json jsonMeta;

	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.empty() && !jsonMeta.is_null())
	{
		std::string libraryPath = jsonMeta.at("library_path");
		if (!std::filesystem::exists(libraryPath))
		{
			CONSOLE_LOG("%s Library file doesn't exist.", errorString.c_str());
			return false;
		}

		UID uid = (UID)jsonMeta.at("uid");
		if (libraryCheck && (library.find(uid) == library.end()))
		{
			CONSOLE_LOG("%s Resource UID not found in library.", errorString.c_str());
			return false;
		}

		if (!jsonMeta.at("contained_resources").empty())
		{
			for (const auto& containedIt : jsonMeta.at("contained_resources").items())
			{
				std::string containedLibraryPath = containedIt.value().at("library_path");
				if (!std::filesystem::exists(containedLibraryPath))
				{
					CONSOLE_LOG("%s Contained library file doesn't exist.", errorString.c_str());
					return false;
				}

				UID containedUid = containedIt.value().at("uid");
				if (libraryCheck && (library.find(containedUid) == library.end()))
				{
					CONSOLE_LOG("%s Contained resource UID not found in library.", errorString.c_str());
					return false;
				}
			}
		}
	}
	else
		CONSOLE_LOG("%s Couldn't load meta file.", errorString.c_str());

	return ret;
}

bool M_ResourceManager::ValidateMetaFile(Json jsonMeta, bool libraryCheck)
{
	bool ret = true;

	std::string errorString = "[ERROR] Resource Manager: validating meta file.";

	if (!jsonMeta.empty() && !jsonMeta.is_null())
	{
		std::string libraryPath = jsonMeta.at("library_path");
		if (!std::filesystem::exists(libraryPath))
		{
			CONSOLE_LOG("%s Library file doesn't exist.", errorString.c_str());
			return false;
		}

		UID uid = (UID)jsonMeta.at("uid");
		if (libraryCheck && (library.find(uid) == library.end()))
		{
			CONSOLE_LOG("%s Resource UID not found in library.", errorString.c_str());
			return false;
		}

		if (!jsonMeta.at("contained_resources").empty())
		{
			for (const auto& containedIt : jsonMeta.at("contained_resources").items())
			{
				std::string containedLibraryPath = containedIt.value().at("library_path");
				if (!std::filesystem::exists(containedLibraryPath))
				{
					CONSOLE_LOG("%s Contained library file doesn't exist.", errorString.c_str());
					return false;
				}

				UID containedUid = containedIt.value().at("uid");
				if (libraryCheck && (library.find(containedUid) == library.end()))
				{
					CONSOLE_LOG("%s Contained resource UID not found in library.", errorString.c_str());
					return false;
				}
			}
		}
	}
	else
		CONSOLE_LOG("%s Couldn't load meta file.", errorString.c_str());

	return ret;
}

bool M_ResourceManager::ResourceHasMetaType(Resource* resource) const
{
	if (resource == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: checking meta type, resource was nullptr.");
		return false;
	}

	switch (resource->GetType())
	{
	case ResourceType::TEXTURE: { return true; } break;
	case ResourceType::MODEL: { return true; } break;
	case ResourceType::MATERIAL: { return true; } break;
	//case ResourceType::FONT: { return true; } break;
	//case ResourceType::PARTICLE: { return true; } break;
	//case ResourceType::TRACK: { return true; } break;
	default:
		break;
	}

	return false;
}

bool M_ResourceManager::HasImportIgnoredExtension(const char* assetPath) const
{
	std::filesystem::path filePath = assetPath;
	return (engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".ini") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".json") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".ttf") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".mp3") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".wav") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".txt") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".milk") == 0 // TODO Temporary (just delete from assets?)
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".bytes") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".bmp") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".svg") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".lua") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".md5mesh") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".md5anim") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".zip") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".rar") == 0
		);
}