#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include <string>

class Resource
{
	friend class ResourceManager;

public:
	enum class Type
	{
		MESH,
		TEXTURE,
		SCENE,
		SHADER,
		UNKNOWN,
	};

	Resource() {}
	Resource(Resource::Type type);
	virtual ~Resource();

	virtual void CleanUp();

	inline Resource::Type GetType() const { return type; }

	inline UID GetUID() const { return uid; }
	inline void SetUID(const UID& uid) { this->uid = uid; }

	inline uint GetReferenceCount() const { return referenceCount; }
	inline void SetReferenceCount(const uint& referenceCount) { this->referenceCount = referenceCount; }

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

	//bool HasResource(UID uid) const;

	//bool LoadToMemory();
	//bool IsLoadedToMemory();

	//virtual void Save() const;
	//virtual void Load();

protected:
	UID uid = 0;
	std::string assetPath = "";
	std::string assetFile = "";
	std::string libraryPath = "";
	std::string libraryFile = "";

	Type type = Type::UNKNOWN;
	uint referenceCount = 0;
};

#endif // !__RESOURCE_H__