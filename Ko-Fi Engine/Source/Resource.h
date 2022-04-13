#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include "json.hpp"

using Json = nlohmann::json;

enum class ResourceType
{
	MESH,
	TEXTURE,
	SCENE,
	FONT,
	TRACK, // TODO
	PARTICLE, // TODO
	MODEL,
	MATERIAL, // TODO
	ANIMATION,
	UNKNOWN
};

class Resource
{
public:
	enum class Type
	{
		MESH,
		TEXTURE,
		SCENE,
		SHADER,
		UNKNOWN,
	};

	Resource();
	Resource(ResourceType type);
	virtual ~Resource();

	virtual bool CleanUp();

	virtual bool SaveMeta(Json& json) const { return true; }
	virtual bool LoadMeta(Json& json) { return true; }

	virtual inline ResourceType GetType() const { return type; }

	inline UID GetUID() const { return uid; }
	inline void SetUID(const UID& uid) { this->uid = uid; }

	inline uint GetReferenceCount() const { return referenceCount; }
	void ModifyReferenceCount(int modification);

	// Getters and Setters for Asset & Library Paths
	inline const char* GetAssetPath() const { return assetPath.c_str(); }
	inline const char* GetAssetFile() const { return assetFile.c_str(); }
	inline void SetAssetPath(const char* assetPath) { this->assetPath = assetPath; }
	inline void SetAssetFile(const char* assetFile) { this->assetFile = assetFile; }

	inline const char* GetLibraryPath() const { return libraryPath.c_str(); }
	inline const char* GetLibraryFile() const { return libraryFile.c_str(); }
	inline void SetLibraryPath(const char* libraryPath) { this->libraryPath = libraryPath; }
	inline void SetLibraryFile(const char* libraryFile) { this->libraryFile = libraryFile; }

	void SetLibraryPathAndFile();
	void SetAssetsPathAndFile(const char* path, const char* file);
	//bool HasResource(UID uid) const;

	//bool LoadToMemory();
	//bool IsLoadedToMemory();

	//virtual void Save() const;
	//virtual void Load();

private:
	UID uid = 0;
	ResourceType type = ResourceType::UNKNOWN;
	std::string assetPath = "";
	std::string assetFile = "";
	std::string libraryPath = "";
	std::string libraryFile = "";

	uint referenceCount = 0;
};

#endif // !__RESOURCE_H__