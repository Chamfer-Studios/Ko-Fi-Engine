#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

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
	FileSystem();
	~FileSystem();

	bool Awake();
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// FileSystem Functions
	bool OpenFile(const char* path) const;
	bool SaveFile(const char* path) const;
	void EnumerateFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs);
	void AddPath(const char* path);
	void GameObjectFromMesh(const char* file_path,std::vector<GameObject*>& meshes);
	void GameObjectFromPrimitive(COMPONENT_SUBTYPE subtype, std::vector<GameObject*>& meshes);

public:
	fs::path rootPath;

};

#endif FILELOADER_H // FILESYSTEM_H