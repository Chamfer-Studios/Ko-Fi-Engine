#include "FileSystem.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "SDL_assert.h"
#include "SDL.h"
#include <fstream>
#include <filesystem>
#include <iomanip>


FileSystem::FileSystem()
{
	name = "ModelLoader";
	std::string rootPathString = SDL_GetBasePath();
	std::replace(rootPathString.begin(), rootPathString.end(), '\\', '/');
	rootPath = rootPathString;
	rootPath = rootPath.parent_path().parent_path();
	AddPath("/Ko-Fi Engine/Ko-Fi");
}

FileSystem::~FileSystem()
{

}

bool FileSystem::Awake()
{
	LOG("Turning on FileSystem debugger...");
	appLog->AddLog("Turning on FileSystem debugger...\n");
	//Prepare filesystem
	//std::filesystem::directory_entry().assign(addPath);
	// Stream log messages to Debug window

	return true;
}

bool FileSystem::Start()
{
	LOG("Starting FileSystem...");
	appLog->AddLog("Starting FileSystem...\n");

	return true;
}

bool FileSystem::PreUpdate(float dt)
{
	return true;
}

bool FileSystem::Update(float dt)
{
	return true;
}

bool FileSystem::PostUpdate(float dt)
{
	return true;
}

bool FileSystem::CleanUp()
{
	LOG("Cleaning FileSystem up...");
	appLog->AddLog("Cleaning FileSystem up...\n");

	// detach log stream
	
	return true;
}

//bool FileSystem::OpenFile(const char* path) const
//{
//	bool ret = true;
//	SDL_assert(path != nullptr);
//	std::ifstream stream(path);
//	stream.is_open() ? ret = true : ret = false;
//	stream.close();
//	return ret;
//}
//
//bool FileSystem::SaveFile(const char* path) const
//{
//	bool ret = true;
//	SDL_assert(path != nullptr);
//	std::ofstream stream(path)
//	stream.is_open() ? ret = true : ret = false;
//	return ret;
//}

void FileSystem::EnumerateFiles(const char* path,std::vector<std::string>&files,std::vector<std::string>&dirs)
{
	std::string p = rootPath.string()+path;
	for (const auto& file : std::filesystem::directory_iterator(p))
	{
		if (std::filesystem::is_directory(file.path())) {
			dirs.push_back(file.path().filename().string());
		}
		else {
			files.push_back(file.path().filename().string());
		}
	}
}

void FileSystem::AddPath(const char* path)
{
	rootPath += path;
}

void FileSystem::LoadMesh(const char* file_path, std::vector<GameObject>& gameObjects)
{
	uint id = gameObjects.size();
	GameObject gameObject = GameObject(file_path, id);
	gameObject.CreateComponent(COMPONENT_TYPE::COMPONENT_MESH);
	gameObjects.push_back(gameObject);
}
