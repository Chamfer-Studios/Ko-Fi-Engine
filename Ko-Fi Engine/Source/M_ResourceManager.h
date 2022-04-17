#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "Module.h"
#include "Globals.h"

class Resource;
class ResourceBase;
enum class ResourceType;

class M_ResourceManager : public Module
{
public:
	M_ResourceManager(KoFiEngine* engine);
	~M_ResourceManager();

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool CleanUp();
	void OnNotify(const Event& event);

	UID ImportFile(const char* assetPath);
	UID LoadFromLibrary(const char* libraryPath);
	UID ImportFromAssets(const char* assetPath);
	void DeleteFromLibrary(const char* assetPath);
	void DeleteFromAssets(const char* assetPath);

	bool TrimLibrary();

	bool HasImportIgnoredExtension(const char* assetPath) const;

	//const Resource* RequestResource(uint uid) const;			Can't do it because of the maps
	bool SaveResource(Resource* resource);
	// Deletes from Library map
	bool UnloadResource(UID uid);
	// Doesnt deletes from Library map
	bool UnloadResource(Resource* resource);

	UID Find(const char* assetPath) const;
	Resource* GetResourceFromLibrary(const char* libraryPath);
	Resource* RequestResource(UID uid);

	ResourceType GetTypeFromAssetsExtension(const char* assetPath);
	
	bool GetAssetDirectoryFromType(const ResourceType& type, std::string& directory);
	bool GetLibraryDirectoryAndExtensionFromType(const ResourceType& type, std::string& directory, std::string& extension);

	std::string GetValidPath(const char* path) const;

	void RefreshDirectoryFiles(const char* directory);
	void FindFilesToImport(std::vector<std::string>& assetsFiles, std::vector<std::string>& metaFiles, std::map<std::string, std::string>& filePairs, std::vector<std::string>& toImport);
	void FindFilesToUpdate(std::map<std::string, std::string>& filePairs, std::vector<std::string>& toUpdate);
	void FindFilesToDelete(std::vector<std::string>& metaFiles, std::map<std::string, std::string>& filePairs, std::vector<std::string>& toDelete);
	void LoadFilesIntoLibrary(std::map<std::string, std::string>& filePairs);

	bool HasMetaFile(const char* assestsPath);
	bool ValidateMetaFile(const char* assetPath, bool libraryCheck = true);
	bool ValidateMetaFile(Json& json, bool libraryCheck = true);
	bool ResourceHasMetaType(Resource* resource) const;

	bool LoadMetaFileIntoLibrary(const char* assetPath);
	bool GetLibraryPairs(const char* assetPath, std::map<UID, ResourceBase>& pairs);
	bool GetForcedUIDsFromMeta(const char* assetPath, std::map<std::string, UID>& uids);
	bool GetResourceUIDsFromMeta(const char* assetPath, std::vector<UID>& uids);
	bool GetResourceBasesFromMeta(const char* assetPath, std::vector<ResourceBase>& bases);
	bool GetLibraryFilePathsFromMeta(const char* assetPath, std::vector<std::string>& paths);
	int GetModTimeFromMeta(const char* assetPath);

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;

	//void ReleaseResource(uint uid);

	Resource* CreateNewResource(const ResourceType& type, const char* assetPath = nullptr, UID forcedUid = 0);

private:
	bool SaveMetaFile(Resource* resource) const;
	bool LoadMetaFile(Json& json, const char* assetPath);

private:
	KoFiEngine* engine = nullptr;

	std::map<UID, Resource*> resourcesMap;
	std::map<UID, ResourceBase> library;

	float fileRefreshRate;
	float fileRefreshTime;
};

#endif // !__RESOURCE_MANAGER_H__