#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Module.h"
#include <filesystem>
#include "GameObject.h"

namespace fs = std::filesystem;

class Renderer3D;

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

class FileSystem : public Module
{
public:
	FileSystem(KoFiEngine* engien);
	~FileSystem();

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

	// FileSystem Functions
	std::string OpenFile(const char* path) const;
	std::string OpenFileBinary(const char* path) const;
	bool SaveFile(const char* path, std::string text) const;
	void EnumerateFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs);
	void AddPath(const char* path);

	void CreateMaterial(const char* path, const char* filename,const char* texturePath);
	void CreateMaterial(const char* path);
	void CreateShader(const char* path);
	void CreateScene(const char* path,const char* sceneName);

public:
	fs::path rootPath;
	KoFiEngine* engine = nullptr;
};

#endif FILESYSTEM_H // !__FILESYSTEM_H__