#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Module.h"
#include <filesystem>
#include "GameObject.h"

namespace fs = std::filesystem;

class M_Renderer3D;

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
	
	void GetLastModTime(const char* path);

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