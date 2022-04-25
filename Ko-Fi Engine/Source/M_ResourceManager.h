#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "Module.h"
#include "Globals.h"

class Resource;
class ResourceBase;
enum class ResourceType;
class R_Material;
class R_Texture;

class M_ResourceManager : public Module
{
public:
	M_ResourceManager(KoFiEngine* engine);
	~M_ResourceManager();

	// Module methods
	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool CleanUp();
	void OnNotify(const Event& event);

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;

	// Import method
	UID ImportFile(const char* assetPath);

	// Saving & Loading resource methods
	bool SaveResource(Resource* resource);
	UID LoadFromLibrary(const char* assetsPath);
	Resource* GetResourceFromLibrary(const char* assetsPath);

	// Resource methods
	Resource* CreateNewResource(const ResourceType& type, const char* assetPath = nullptr, UID forcedUid = 0);
	Resource* RequestResource(UID uid);						//
	bool LoadResource(UID uid, const char* assetsPath);		//
	bool UnloadResource(UID uid);							// Deletes from Library map
	bool UnloadResource(Resource* resource);				// Doesnt deletes from Library map

	UID Find(const char* assetPath) const;					//
	void FindAndForceUID(Resource* resource);				//

	// Meta file methods to get uids
	UID GetForcedUIDFromMeta(const char* assetPath);
	bool GetForcedUIDsFromMeta(const char* assetPath, std::map<std::string, UID>& uids);

	// Methods to import resources
	bool ImportMaterial(const char* assetsPath, R_Material* material);
	bool ImportTexture(const char* assetsPath, R_Texture* texture);

private:
	void RefreshDirectoryFiles(const char* directory);
	void FindFilesToImport(std::vector<std::string>& assetsFiles, std::vector<std::string>& metaFiles, std::map<std::string, std::string>& filePairs, std::vector<std::string>& toImport);
	void FindFilesToUpdate(std::map<std::string, std::string>& filePairs, std::vector<std::string>& toUpdate);
	void FindFilesToDelete(std::vector<std::string>& metaFiles, std::map<std::string, std::string>& filePairs, std::vector<std::string>& toDelete);
	
	void LoadFilesIntoLibrary(std::map<std::string, std::string>& filePairs);

	void DeleteFromLibrary(const char* assetPath);
	void DeleteFromAssets(const char* assetPath);

	bool TrimLibrary();

	bool GetResourceUIDsFromMeta(const char* assetPath, std::vector<UID>& uids);
	bool GetResourceBasesFromMeta(const char* assetPath, std::vector<ResourceBase>& bases);
	bool GetLibraryFilePathsFromMeta(const char* assetPath, std::vector<std::string>& paths);

	bool GetAssetDirectoryFromType(const ResourceType& type, std::string& directory);
	bool GetLibraryDirectoryAndExtensionFromType(const ResourceType& type, std::string& directory, std::string& extension);

	bool LoadMetaLibraryPairsIntoLibrary(const char* assetPath);
	bool GetLibraryPairs(const char* assetPath, std::map<UID, ResourceBase>& pairs);
	int GetModTimeFromMeta(const char* assetPath);

	UID ImportFromAssets(const char* assetPath);

	std::string GetValidPath(const char* path) const;
	ResourceType GetTypeFromPathExtension(const char* path);

	bool SaveMetaFile(Resource* resource) const;
	bool HasMetaFile(const char* assestsPath);
	bool ValidateMetaFile(const char* assetPath, bool libraryCheck = true);
	bool ResourceHasMetaType(Resource* resource) const;

	bool HasImportIgnoredExtension(const char* assetPath) const;

private:
	KoFiEngine* engine = nullptr;

	std::map<UID, Resource*> resourcesMap;
	std::map<UID, ResourceBase> library;

	float fileRefreshRate;
	float fileRefreshTime;
};

#endif // !__RESOURCE_MANAGER_H__