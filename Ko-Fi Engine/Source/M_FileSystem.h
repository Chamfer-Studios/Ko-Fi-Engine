#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Module.h"
#include <filesystem>

namespace fs = std::filesystem;

class M_Renderer3D;
class GameObject;
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

class M_FileSystem : public Module
{
public:
	M_FileSystem(KoFiEngine* engien);
	~M_FileSystem();

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	// Method to receive and manage events
	void OnNotify(const Event& event);

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------

	// M_FileSystem Functions
	std::string OpenFile(const char* path) const;
	std::string OpenFileBinary(const char* path) const;
	bool SaveFile(const char* path, std::string text) const;

	void EnumerateFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs);
	void DiscoverAllFiles(const char* directory, std::vector<std::string>& files);
	void DiscoverAllFilesFiltered(const char* directory, std::vector<std::string>& files, std::vector<std::string>& filteredFiles, const char* filter);
	
	// Checks if a directory already exists, if not, it creates it accordingly. Return true either the directory already exists or it was created, and return false if any error occurred.
	bool CheckDirectory(const char* path);
	
	// Returns the last modification time in seconds from the epoch time. The Greater the number returned, the more recent the modification is.
	int GetLastModTime(const char* path);

	const char* GetFileName(const char* path) const;
	std::string GetNameFromPath(std::string path);
	
	int StringCompare(const char* a, const char* b);
	void AddPath(const char* path);

	void CreateMaterial(const char* path, const char* filename, const char* texturePath);
	void CreateMaterial(const char* path);
	void CreateShader(const char* path);
	void CreateScene(const char* path,const char* sceneName);

public:
	fs::path rootPath;
	KoFiEngine* engine = nullptr;
};

#endif FILESYSTEM_H // !__FILESYSTEM_H__