#include "M_ResourceManager.h"
#include "Engine.h"
#include "Log.h"
#include "RNG.h"
#include "FSDefs.h"
#include "Importer.h"
#include "M_FileSystem.h"
#include "ResourceBase.h"

#include "json.hpp"
#include "JsonHandler.h"
using Json = nlohmann::json;

#include <filesystem>

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

	//RefreshDirectoryFiles(ASSETS_DIR);
	//RefreshDirectoryFiles(LIBRARY_DIR);

	//TrimLibrary();

	//Find prefabs

	return true;
}

bool M_ResourceManager::PreUpdate(float dt)
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

bool M_ResourceManager::CleanUp()
{
	CONSOLE_LOG("Cleaning M_ResourceManager up...");
	//appLog->AddLog("Cleaning M_ResourceManager up...\n");

	bool ret = true;

	//std::map<UID, Resource*>::iterator it;
	//for (it = resourcesMap.begin(); it != resourcesMap.end(); ++it)
	//{
	//	it->second->CleanUp();
	//	RELEASE(it->second);
	//}

	//resourcesMap.clear();
	//library.clear();

	return ret;
}

// Method to receive and manage events
void M_ResourceManager::OnNotify(const Event& event)
{
	// Manage events
}

bool M_ResourceManager::TrimLibrary()
{
	std::vector<std::string> files;
	engine->GetFileSystem()->DiscoverAllFiles(LIBRARY_DIR, files);

	std::map<std::string, UID> fileUIDs;
	for (auto file = files.cbegin(); file != files.cend(); ++file)
	{
		UID uid = 0;
		sscanf(engine->GetFileSystem()->GetFileName((*file).c_str()), "%u", &uid);

		if (uid != 0)
			fileUIDs.emplace((*file), uid);
	}

	for ( std::map<std::string,UID>::const_iterator UID = fileUIDs.cbegin(); UID != fileUIDs.cend(); ++UID)
	{
		if (library.find(UID->second) == library.end())
		{
			std::filesystem::remove(UID->first.c_str());
		}
	}

	fileUIDs.clear();

	files.clear();
	files.shrink_to_fit();

	return true;
}

UID M_ResourceManager::ImportFile(const char* assetPath)
{
	UID uid = 0;
	if (assetPath == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: loading file, file path was nullptr.");
		return uid;
	}

	if (HasImportIgnoredExtension(assetPath))
	{
		LOG_BOTH("[ERROR] Resource Manager: loading file, the file extension has an import ignored extension.");
		return uid;
	}

	std::filesystem::path path = assetPath;
	ResourceType type = GetTypeFromExtension(path.extension().string().c_str());

	if (type == ResourceType::UNKNOWN)
	{
		LOG_BOTH("[ERROR] Resource Manager: loading file, unknown file type.");
		return uid;
	}

	std::string cleanPath = GetValidPath(assetPath);
	if (cleanPath.c_str() == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: loading file, couldn't validate path.");
		return uid;
	}

	bool metaIsValid = ValidateMetaFile(assetPath);
	if (metaIsValid)
	{
		LOG_BOTH("File to import was already in the library.");

		std::map<UID, ResourceBase> libraryItems;
		GetLibraryPairs(assetPath, libraryItems);

		for (auto& item : libraryItems)
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
			LOG_BOTH("[ERROR] Resource Manager: loading file, error loading file from assets.");
	}

	return uid;
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

	std::vector<std::string> assetFiles;
	std::vector<std::string> metaFiles;
	std::map<std::string, std::string> filePairs;

	engine->GetFileSystem()->DiscoverAllFilesFiltered(directory, assetFiles, metaFiles, META_EXTENSION);

	FindFilesToImport(assetFiles, metaFiles, filePairs, toImport);
	FindFilesToUpdate(filePairs, toUpdate);
	FindFilesToDelete(metaFiles, filePairs, toDelete);

	LoadFilesIntoLibrary(filePairs);

	filePairs.clear();
	metaFiles.clear();
	metaFiles.shrink_to_fit();
	assetFiles.clear();
	assetFiles.shrink_to_fit();

	for (uint i = 0; i < toDelete.size(); ++i)
		DeleteFromLibrary(toDelete[i].c_str());

	for (uint i = 0; i < toUpdate.size(); ++i)
	{
		std::string extension = toUpdate[i].c_str();
		extension = extension.substr(extension.find_last_of("."), extension.size());

		if (extension == "h" || extension == "particles" || extension == "navmesh" || extension == "shader" || extension == "png")
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

void M_ResourceManager::FindFilesToImport(std::vector<std::string>& assetFiles, std::vector<std::string>& metaFiles, std::map<std::string, std::string>& filePairs, std::vector<std::string>& toImport)
{
	if (assetFiles.empty())
		return;

	std::string metaFile = "";
	for (uint i = 0; i < assetFiles.size(); ++i)
	{
		if (HasImportIgnoredExtension(assetFiles[i].c_str()))
			continue;

		metaFile = assetFiles[i] + META_EXTENSION;
		bool find = false;
		for (uint j = 0; j < metaFiles.size(); ++j)
		{
			if (metaFile == metaFiles[j])
				find = true;
		}

		if (find)
		{
			filePairs.emplace(assetFiles[i], metaFile);

			if (!ValidateMetaFile(assetFiles[i].c_str(), false))
				toImport.push_back(assetFiles[i]);
		}
		else
			toImport.push_back(assetFiles[i]);
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
		auto assetModification = std::filesystem::last_write_time(item.first);
		auto metaModification = std::filesystem::last_write_time(item.second);
		if (assetModification != metaModification)
		{
			CONSOLE_LOG("Modification time discrepancy with file %s\n", item.first);
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
		assetPath = metaFiles[i].substr(0,assetPath.find_last_of(META_EXTENSION) - 5);
		if (filePairs.find(assetPath) == filePairs.end())
		{
			toDelete.push_back(assetPath);
		}
	}
}

void M_ResourceManager::LoadFilesIntoLibrary(std::map<std::string, std::string>& filePairs)
{
	if (filePairs.empty())
		return;

	for (const auto& item : filePairs)
	{
		LoadMetaFileIntoLibrary(item.first.c_str());
	}
}

bool M_ResourceManager::LoadMetaFileIntoLibrary(const char* assetsPath)
{
	if (assetsPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: loading meta file into library, assets path was nullptr.");
		return false;
	}

	std::map<UID, ResourceBase> libraryPairs;
	GetLibraryPairs(assetsPath, libraryPairs);

	if (libraryPairs.empty())
		return true;

	for (auto item : libraryPairs)
		library.emplace(item.first, item.second);

	libraryPairs.clear();

	return true;
}

bool M_ResourceManager::GetLibraryPairs(const char* assetsPath, std::map<UID, ResourceBase>& pairs)
{
	if (assetsPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting library pairs, assets path was nullptr.");
		return false;
	}

	std::string errorString = "[ERROR] Resource Manager: could not get library file pairs from { " + std::string(assetsPath) + " }'s meta file.";

	std::vector<UID> resourceUIDs;
	std::vector<ResourceBase> bases;
	GetResourceUIDsFromMeta(assetsPath, resourceUIDs);
	GetResourceBasesFromMeta(assetsPath, bases);

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

bool M_ResourceManager::GetResourceUIDsFromMeta(const char* assetsPath, std::vector<UID>& uids)
{
	bool ret = true;

	if (assetsPath == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: getting UIDs from meta, assets path was nullptr.");
		return false;
	}

	JsonHandler jsonHandler;
	Json jsonMeta;

	std::string metaPath = assetsPath + std::string(META_EXTENSION);
	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		UID uid = jsonMeta.at("uid");
		if (uid == 0)
		{
			LOG_BOTH("[ERROR] Resource Manager: main resource UID was 0.");
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
						LOG_BOTH("[ERROR] Resource Manager: contained resource UID was 0.");
						continue;
					}
					uids.push_back(containedUid);
				}
			}
		}
	}
	return true;
}

bool M_ResourceManager::GetResourceBasesFromMeta(const char* assetsPath, std::vector<ResourceBase>& bases)
{
	bool ret = true;

	if (assetsPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting resource bases from meta, assets path was nullptr.");
		return false;
	}

	std::string errorString = "[ERROR] Resource Manager: could not get library file paths from { " + std::string(assetsPath) + " }'s meta file";

	JsonHandler jsonHandler;
	Json jsonMeta;

	std::string metaPath = assetsPath + std::string(META_EXTENSION);
	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		UID uid = jsonMeta.at("uid");
		ResourceType type = (ResourceType)(int)jsonMeta.at("type");
		std::string assetsPath = jsonMeta.at("assets_path");
		std::string assetsFile = jsonMeta.at("assets_file");
		std::string libraryPath = jsonMeta.at("library_path");
		std::string libraryFile = jsonMeta.at("library_file");

		if (uid == 0)
		{
			CONSOLE_LOG("[ERROR] Resource Manager: main resource UID was 0.");
			return false;
		}
		bases.push_back(ResourceBase(uid, type, assetsPath, assetsFile, libraryPath, libraryFile));

		if (jsonMeta.contains("contained_resources"))
		{
			if (!jsonMeta.at("contained_resources").is_null() && !jsonMeta.at("contained_resources").empty())
			{
				for (const auto& containedIt : jsonMeta.at("contained_resources").items())
				{
					UID containedUid = containedIt.value().at("uid");
					ResourceType containedType = (ResourceType)(int)jsonMeta.at("type");

					std::string containedAssetsPath = "";
					//std::string containedAssetsPath = jsonMeta.at("assets_path");
					std::string containedAssetsFile = jsonMeta.at("assets_file");

					std::string containedLibraryPath = jsonMeta.at("library_path");
					std::string containedLibraryFile = "";
					//std::string containedLibraryFile = jsonMeta.at("library_file");

					std::string directory = "";
					bool success = GetAssetDirectoryFromType(containedType, directory);
					if (!success)
						continue;

					if (containedUid == 0)
					{
						LOG_BOTH("[ERROR] Resource Manager: contained resource UID was 0.");
						continue;
					}

					containedAssetsPath = directory + containedAssetsFile;
					containedLibraryFile = engine->GetFileSystem()->GetFileName(containedLibraryPath.c_str());
					bases.push_back(ResourceBase(containedUid, containedType, containedAssetsPath, containedAssetsFile, containedLibraryPath, containedLibraryFile));
				}
			}
		}
	}
	else
		CONSOLE_LOG("%s Asset path had no associated meta file.", errorString.c_str());

	return ret;
}

bool M_ResourceManager::GetLibraryFilePathsFromMeta(const char* assetsPath, std::vector<std::string>& paths)
{
	bool ret = true;

	if (assetsPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: getting library file paths from meta, assets path was nullptr.");
		return false;
	}

	std::string errorString = "[ERROR] Resource Manager: could not get library file paths from { " + std::string(assetsPath) + " }'s meta file";

	JsonHandler jsonHandler;
	Json jsonMeta;

	std::string metaPath = assetsPath + std::string(META_EXTENSION);
	ret = jsonHandler.LoadJson(jsonMeta, metaPath.c_str());

	if (ret && !jsonMeta.is_null() && !jsonMeta.empty())
	{
		UID uid = jsonMeta.at("uid");
		ResourceType type = (ResourceType)(int)jsonMeta.at("type");

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
				for (const auto& containedIt : jsonMeta.at("contained_resources").items())
				{
					UID containedUid = containedIt.value().at("uid");
					ResourceType containedType = (ResourceType)(int)jsonMeta.at("type");

					//std::string containedAssetsPath = "";
					////std::string containedAssetsPath = jsonMeta.at("assets_path");
					//std::string containedAssetsFile = jsonMeta.at("assets_file");

					//std::string containedLibraryPath = jsonMeta.at("library_path");
					//std::string containedLibraryFile = "";
					////std::string containedLibraryFile = jsonMeta.at("library_file");

					//std::string directory = "";
					//bool success = GetAssetDirectoryFromType(containedType, directory);
					//if (!success)
					//	continue;

					//if (containedUid == 0)
					//{
					//	LOG_BOTH("[ERROR] Resource Manager: contained resource UID was 0.");
					//	continue;
					//}

					//containedAssetsPath = directory + containedAssetsFile;
					//containedLibraryFile = engine->GetFileSystem()->GetFileName(containedLibraryPath.c_str());
					//bases.push_back(ResourceBase(containedUid, containedType, containedAssetsPath, containedAssetsFile, containedLibraryPath, containedLibraryFile));
				}
			}
		}
	}

	return true;
}

void M_ResourceManager::DeleteFromLibrary(const char* assetsPath)
{
	if (assetsPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: deleting file from library, assets path was nullptr.");
		return;
	}

	if (!HasMetaFile(assetsPath))
	{
		CONSOLE_LOG("[ERROR] Resource Manager: deleting file from library, %s file couldn't be found or doesn't exist.", assetsPath);
		return;
	}

	std::vector<UID> resourceUids;
	std::vector<std::string> toDelete;

	GetResourceUIDsFromMeta(assetsPath, resourceUids);
	GetLibraryFilePathsFromMeta(assetsPath, toDelete);

	for (uint i = 0; i < resourceUids.size(); ++i)
	{
		UnloadResource(resourceUids[i]);
	}

	for (uint i = 0; i < toDelete.size(); ++i)
	{
		std::filesystem::remove(toDelete[i].c_str());
	}

	toDelete.clear();
	toDelete.shrink_to_fit();
	resourceUids.clear();
	resourceUids.shrink_to_fit();

}

bool M_ResourceManager::HasMetaFile(const char* assetsPath)
{
	if (assetsPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: checking for meta file, assets path was nullptr.");
		return false;
	}

	std::string path = assetsPath + std::string(META_EXTENSION);
	return std::filesystem::exists(path);
}

bool M_ResourceManager::ValidateMetaFile(const char* assetsPath, bool libraryCheck)
{
	bool ret = true;

	if (assetsPath == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: validating meta file, assetsPath was nullptr.");
		return false;
	}

	std::string metaPath = assetsPath + std::string(META_EXTENSION);

	std::string errorString = "[ERROR] Resource Manager: validating meta file: " + metaPath;

	if (!std::filesystem::exists(metaPath))
	{
		CONSOLE_LOG("%s Couldn't find meta file.", errorString.c_str());
		return false;
	}

	JsonHandler jsonHandler;
	Json jsonMeta;

	ret = jsonHandler.LoadJson(jsonMeta, assetsPath);

	if (ret && !jsonMeta.is_null())
	{
		std::string libraryPath = jsonMeta.at("library_path");

		if (!std::filesystem::exists(libraryPath))
		{
			CONSOLE_LOG("%s Validating library path, file doesn't exist.", errorString.c_str());
			return false;
		}

		UID uid = (UID)jsonMeta.at("uid");

		if (libraryCheck && (library.find(uid) == library.end()))
		{
			CONSOLE_LOG("%s Resource UID not found in library", errorString.c_str());
			return false;
		}

		if (!jsonMeta.at("contained_resources").empty())
		{
			for (const auto& resource : jsonMeta.at("contained_resources").items())
			{
				UID containedUid = resource.value().at("uid");

				std::string containedLibraryPath = jsonMeta.at("library_path");

				if (!std::filesystem::exists(containedLibraryPath))
				{
					CONSOLE_LOG("%s Validating contained library path, file doesn't exist.", errorString.c_str());
					return false;
				}
				if (libraryCheck && (library.find(containedUid) == library.end()))
				{
					CONSOLE_LOG("%s Contained resource UID not found in library", errorString.c_str());
					return false;
				}
			}
		}
	}
	else
		CONSOLE_LOG("%s, Loading meta file.", errorString.c_str());

	return ret;
}

bool M_ResourceManager::ValidateMetaFile(Json& json, bool libraryCheck)
{
	bool ret = true;

	std::string errorString = "[ERROR] Resource Manager: could not validate meta file";

	if (json.empty() || json.is_null())
	{
		CONSOLE_LOG("%s Json is null or empty.", errorString.c_str());
		return false;
	}

	std::string libraryPath = json.at("library_path");

	if (!std::filesystem::exists(libraryPath))
	{
		CONSOLE_LOG("%s Library path, file doesn't exist.", errorString.c_str());
		return false;
	}

	UID uid = (UID)json.at("uid");

	if (libraryCheck && (library.find(uid) == library.end()))
	{
		CONSOLE_LOG("%s Resource UID not found in library", errorString.c_str());
		return false;
	}

	if (!json.at("contained_resources").empty())
	{
		for (const auto& resource : json.at("contained_resources").items())
		{
			UID containedUid = resource.value().at("uid");

			std::string containedLibraryPath = json.at("library_path");

			if (!std::filesystem::exists(containedLibraryPath))
			{
				CONSOLE_LOG("%s Contained Library path, file doesn't exist.", errorString.c_str());
				return false;
			}
			if (libraryCheck && (library.find(containedUid) == library.end()))
			{
				CONSOLE_LOG("%s Contained resource UID not found in library", errorString.c_str());
				return false;
			}
		}
	}

	return ret;
}

bool M_ResourceManager::ResourceHasMetaType(Resource* resource) const
{
	if (resource == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: checking meta type, resource was nullptr.");
		return false;
	}

	switch (resource->GetType())
	{
	case ResourceType::TEXTURE: { return true; } break;
	case ResourceType::MATERIAL: { return true; } break;
	case ResourceType::PARTICLE: { return true; } break;
	case ResourceType::TRACK: { return true; } break;
	// TODO: REMAINING TYPES (?)
	default: break;
	}

	return false;
}

Resource* M_ResourceManager::CreateNewResource(const char* assetPath, ResourceType type)
{
	Resource* ret = new Resource(type);

	resourcesMap[ret->GetUID()] = ret;
	ret->SetAssetsPathAndFile(assetPath, engine->GetFileSystem()->GetFileName(assetPath));
	ret->SetLibraryPathAndFile();
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
	jsonMeta["name"] = resource->GetAssetFile();
	jsonMeta["assets_path"] = resource->GetAssetPath();
	jsonMeta["assets_file"] = resource->GetAssetFile();
	jsonMeta["library_file"] = resource->GetLibraryFile();
	jsonMeta["library_path"] = resource->GetLibraryPath();
	//TODO: ADD MODIFICATION TIME

	//TODO: Make SaveMeta / LoadMeta inherit for all resources with meta (to custom its save / load)
	resource->SaveMeta(jsonMeta);

	std::string path = resource->GetAssetPath() + std::string(META_EXTENSION);

	JsonHandler jsonHandler;
	ret = jsonHandler.SaveJson(jsonMeta, path.c_str());

	return ret;
}

bool M_ResourceManager::LoadMetaFile(Json& json, const char* assetPath)
{
	bool ret = true;

	if (assetPath == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: loading meta file, assetPath was nullptr.");
		return false;
	}

	//TODO: I don't know what to do here, I think it's this:
	// engine->GetFileSystem()->OpenFile(assetPath);
	UID uid = (UID)json.at("uid");
	Resource* r = RequestResource(uid);

	if (r == nullptr)
	{
		LOG_BOTH("Error loading meta file, resource was not found.");
		return false;
	}

	ResourceType type = (ResourceType)json.at("type");
	if (r->GetType() != type)
	{
		LOG_BOTH("Error loading meta file, resource type missmatch.");
		return false;
	}


	r->SetUID(uid);
	r->SetAssetFile(json.at("name").get<std::string>().c_str());
	r->SetAssetPath(json.at("assets_path").get<std::string>().c_str());
	r->SetLibraryFile(json.at("library_file").get<std::string>().c_str());
	r->SetLibraryPath(json.at("library_path").get<std::string>().c_str());

	//r->LoadMeta(json); // I'm not sure about this since the function is empty

	return true;
}

Resource* M_ResourceManager::RequestResource(UID uid)
{
	// Find if the resource is already loaded
	std::map<UID, Resource*>::iterator it = resourcesMap.find(uid);

	if (it != resourcesMap.end())
	{
		it->second->ModifyReferenceCount(1);
		return it->second;
	}

	auto libIt = library.find(uid);
	if (libIt != library.end())
	{
		return GetResourceFromLibrary(libIt->second.assetsPath.c_str());
	}
	return nullptr;
}

UID M_ResourceManager::Find(const char* assetPath) const
{
	for (auto r : resourcesMap)
	{
		if (r.second->GetAssetPath() == assetPath)
			return r.first;
	}
	return -1;
}

void M_ResourceManager::SaveResource(Resource* resource)
{
	if (resource == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: saving resource, resource was nullptr.");
		return;
	}

	switch (resource->GetType())
	{
	case ResourceType::MESH:
		Importer::GetInstance()->meshImporter->Save((R_Mesh*)resource, resource->GetLibraryPath());
		break;
	case ResourceType::SCENE:
		Importer::GetInstance()->sceneImporter->Save((Scene*)resource);
		break;
	case ResourceType::FONT:
		//TODO: Save Font
		break;
	default:
		break;
	}

	if (ResourceHasMetaType(resource))
		SaveMetaFile(resource);

	library.emplace(resource->GetUID(), ResourceBase(resource));
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
		CONSOLE_LOG("[ERROR] Resource Manager: trying to unload resource, unloaded resource was not inside map!");
		return false;
	}
	return true;
}

bool M_ResourceManager::UnloadResource(UID uid)
{
	if (library.find(uid) != library.end())
		library.erase(uid);

	if (resourcesMap.find(uid) != resourcesMap.end())
	{
		Resource* r = resourcesMap[uid];
		r->CleanUp();
		RELEASE(r);
		resourcesMap.erase(uid);
	}
	else
	{
		CONSOLE_LOG("[ERROR] Resource Manager: trying to unload resource, resource was not inside map!");
		return false;
	}
	return true;
}

Resource* M_ResourceManager::GetResourceFromLibrary(const char* libraryPath)
{
	if (libraryPath == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: getting resource, library path was nullptr.");
		return nullptr;
	}

	UID uid = LoadFromLibrary(libraryPath);
	if (uid == 0)
	{
		LOG_BOTH("[ERROR] Resource Manager: getting resource from library, could not get resource uid from assests path.");
		return nullptr;
	}

	Resource* resource = RequestResource(uid);
	if (resource == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: getting resource from library, could not request resource.");
	}

	return resource;
}

UID M_ResourceManager::LoadFromLibrary(const char* libraryPath)
{
	std::string cleanPath = GetValidPath(libraryPath);
	if (cleanPath.c_str() == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: loading from library, couldn't validate path.");
		return 0;
	}

	Json jsonRoot;
	LoadMetaFile(jsonRoot, libraryPath);
	bool metaIsValid = ValidateMetaFile(jsonRoot);

	if (jsonRoot.empty())
	{
		LOG_BOTH("[ERROR] Resource Manager: loading from library, could not get the meta root node.");
		return 0;
	}
	if (!metaIsValid)
	{
		LOG_BOTH("[ERROR] Resource Manager: loading from library, could not validate meta root node.");
		return 0;
	}

	UID uid = (UID)jsonRoot.at("uid");

	if (resourcesMap.find(uid) != resourcesMap.end())
		return uid;

	//TODO: Allocate resource function

	for (const auto& resource : jsonRoot.at("resources").items())
	{
		std::string containedPath;
		std::string containedName = resource.value().at("name").get<std::string>();
		UID containedUid = (UID)resource.value().at("uid");

		//TODO: I don't know how to do this
		//
		//App->fileSystem->SplitFilePath(assetsPath, &containedPath, nullptr, nullptr);
		//containedName = containedNode.GetString("Name");
		//containedPath += containedName;

		if (resourcesMap.find(containedUid) != resourcesMap.end())
		{
			continue;
		}

		//TODO: Allocate resource function
		containedPath.clear();
		containedPath.shrink_to_fit();
		containedName.clear();
		containedName.shrink_to_fit();
	}

	return uid;
}

UID M_ResourceManager::ImportFromAssets(const char* assetsPath)
{
	UID uid;

	if (assetsPath == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: loading from assets, path was nullptr.");
		return -1;
	}

	//TODO: META SHIT
	//char* buffer = nullptr;
	//uint read = App->fileSystem->Load(assetsPath, &buffer);
	//if (read == 0)
	//{
	//	LOG("[ERROR] Resource Manager: Could not Import File %s! Error: File System could not Read the File.", assetsPath);
	//	return 0;
	//}

	ResourceType type = GetTypeFromExtension(assetsPath);
	Resource* resource = CreateNewResource(assetsPath, type);

	switch (type)
	{
	case ResourceType::TEXTURE:
		Importer::GetInstance()->textureImporter->Import(assetsPath,(R_Texture*)resource);
		break;
	case ResourceType::FONT:
		//TODO: FONT IMPORT
		break;
	case ResourceType::TRACK:
		//TODO: TRACK IMPORT
		break;
	case ResourceType::PARTICLE:
		//TODO: PARTICLE IMPORT
		break;
	case ResourceType::MODEL:
		//TODO: MODEL IMPORT
		break;
	case ResourceType::MATERIAL:
		//TODO: MATERIAL IMPORT
		break;
	case ResourceType::ANIMATION:
		//TODO: ANIMATION IMPORT
		break;
	default:
		break;
	}

	//RELEASE_ARRAY(buffer);

	uid = resource->GetUID();
	SaveResource(resource);
	UnloadResource(resource);

	return uid;
}

ResourceType M_ResourceManager::GetTypeFromExtension(const char* extension)
{
	ResourceType ret = ResourceType::UNKNOWN;

	// TODO
	//LUA?
	if (engine->GetFileSystem()->StringCompare(extension, TEXTURE_EXTENSION) == 0)
		ret = ResourceType::TEXTURE;
	else if (engine->GetFileSystem()->StringCompare(extension, MODEL_EXTENSION) == 0)
		ret = ResourceType::MODEL;
	else if (engine->GetFileSystem()->StringCompare(extension, SCENE_EXTENSION) == 0)
		ret = ResourceType::SCENE;
	else if (engine->GetFileSystem()->StringCompare(extension, SHADER_EXTENSION) == 0)
		ret = ResourceType::MATERIAL;
	else if (engine->GetFileSystem()->StringCompare(extension, FONT_EXTENSION) == 0)
		ret = ResourceType::FONT;
	else if (engine->GetFileSystem()->StringCompare(extension, MESH_EXTENSION) == 0)
		ret = ResourceType::MESH;
	else if (engine->GetFileSystem()->StringCompare(extension, ANIMATION_EXTENSION) == 0)
		ret = ResourceType::ANIMATION;

	return ret;
}

bool M_ResourceManager::GetAssetDirectoryFromType(const ResourceType& type, std::string& directory)
{
	switch (type)
	{
	case ResourceType::MODEL: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::MESH: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::TEXTURE: { directory = ASSETS_TEXTURES_DIR; } break;
	case ResourceType::SCENE: { directory = ASSETS_SCENES_DIR; } break;
	//case ResourceType::TRACK: { directory = ASSETS_AUDIO_DIR; } break;
	case ResourceType::FONT: { directory = ASSETS_FONTS_DIR; } break;
	//case ResourceType::PARTICLE: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::ANIMATION: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::MATERIAL: { directory = ASSETS_SHADERS_DIR; } break;
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
	case ResourceType::TEXTURE: { directory = TEXTURES_DIR; extension = TEXTURE_EXTENSION; } break;
	case ResourceType::SCENE: { directory = SCENES_DIR; extension = SCENE_EXTENSION; } break;
	//case ResourceType::TRACK: { directory = AUDIOS_DIR; extension = MODEL_EXTENSION; } break;
	case ResourceType::FONT: { directory = FONTS_DIR; extension = FONT_EXTENSION; } break;
	//case ResourceType::PARTICLE: { directory = ASSETS_MODELS_DIR; } break;
	case ResourceType::ANIMATION: { directory = ANIMATIONS_DIR; extension = ANIMATION_EXTENSION; } break;
	case ResourceType::MATERIAL: { directory = SHADERS_DIR;  extension = SHADER_EXTENSION; } break;
	case ResourceType::UNKNOWN:
	{
		CONSOLE_LOG("[ERROR] Resource Manager: couldn't return library directory from type because it is UNKNOWN.");
		return false;
	}
	break;
	default: break;
	}

	return true;
}

void M_ResourceManager::DeleteFromAssets(const char* assetsPath)
{
	if (assetsPath == nullptr)
	{
		LOG_BOTH("[ERROR] Resource Manager: deleting from assets, assets path was nullptr.");
		return;
	}

	std::vector<UID> resourceUIDs;
	std::vector<std::string> toDelete;

	GetResourceUIDsFromMeta(assetsPath, resourceUIDs);
	GetLibraryFilePathsFromMeta(assetsPath, toDelete);

	std::string metaPath = assetsPath + std::string(META_EXTENSION);
	toDelete.push_back(assetsPath);
	toDelete.push_back(metaPath);

	for (uint i = 0; i < resourceUIDs.size(); ++i)
	{
		UnloadResource(resourceUIDs[i]);
	}

	for (uint i = 0; i < toDelete.size(); ++i)
	{
		std::filesystem::remove(toDelete[i].c_str());
	}

	toDelete.clear();
	toDelete.shrink_to_fit();
	resourceUIDs.clear();
	resourceUIDs.shrink_to_fit();
}

std::string M_ResourceManager::GetValidPath(const char* path) const
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
		resultPath = normalizedPath.substr(assetStart, normalizedPath.size()).c_str();
	else if (libraryStart != std::string::npos)
		resultPath = normalizedPath.substr(libraryStart, normalizedPath.size()).c_str();
	else
		LOG_BOTH("[ERROR] Resource Manager: Couldn't validate path.");

	return resultPath;
}

bool M_ResourceManager::HasImportIgnoredExtension(const char* assetsPath) const
{
	std::filesystem::path filePath = assetsPath;
	return (engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".ini") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".json") == 0
		|| engine->GetFileSystem()->StringCompare(filePath.extension().string().c_str(), ".ttf") == 0);
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