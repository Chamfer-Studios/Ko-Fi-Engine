#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include <string>

class Resource 
{
	friend class ResourceManager;

public:
	enum class Type {
		MESH,
		TEXTURE,
		SCENE,
		SHADER,
		UNKNOWN,
	};

	Resource() {}
	Resource(UID uid, Resource::Type type, const char* assetPath);
	~Resource();

	inline Resource::Type GetType() const { return type; }
	inline UID GetUID() const { return uid; }
	inline const char* GetAssetPath() const { return assetPath.c_str(); }
	inline const char* GetLibraryPath() const { return libraryPath.c_str(); }
	inline const char* GetName() const { return name.c_str(); }

	bool HasResource(UID uid) const;

	//uint GetReferenceCount() const;

	//bool LoadToMemory();
	//bool IsLoadedToMemory();

	//virtual void Save() const;
	//virtual void Load();

protected:
	UID uid = 0;
	std::string name = "";
	std::string assetPath = "";
	std::string libraryPath = "";

	Type type = Type::UNKNOWN;
	uint referenceCount = 0;
};

#endif __RESOURCE_H__