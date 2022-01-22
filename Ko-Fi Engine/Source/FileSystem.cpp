#include "FileSystem.h"
#include "Engine.h"
#include "SceneIntro.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "SDL_assert.h"
#include "SDL.h"
#include <fstream>
#include <filesystem>
#include <iomanip>
#include "JsonHandler.h"
FileSystem::FileSystem(KoFiEngine* engine)
{
	name = "ModelLoader";
	std::string rootPathString = SDL_GetBasePath();
	std::replace(rootPathString.begin(), rootPathString.end(), '\\', '/');
	rootPath = rootPathString;
	// Comment this for release path and uncomment when developing...
	rootPath = rootPath.parent_path().parent_path();
	// Comment this for release path and uncomment when developing...
	AddPath("/Ko-Fi Engine/Ko-Fi");
	this->engine = engine;

	CONSOLE_LOG("Filesystem: %s", rootPath.string());
	appLog->AddLog("Filesystem: %s\n", rootPath.string());
}

FileSystem::~FileSystem()
{

}

bool FileSystem::Awake()
{
	CONSOLE_LOG("Turning on FileSystem debugger...");
	appLog->AddLog("Turning on FileSystem debugger...\n");
	//Prepare filesystem
	//std::filesystem::directory_entry().assign(addPath);
	// Stream log messages to Debug window

	return true;
}

bool FileSystem::Start()
{
	CONSOLE_LOG("Starting FileSystem...");
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
	CONSOLE_LOG("Cleaning FileSystem up...");
	appLog->AddLog("Cleaning FileSystem up...\n");

	// detach log stream

	return true;
}

std::string FileSystem::OpenFile(const char* path) const
{
	std::string fileText;

	SDL_assert(path != nullptr);
	std::ifstream stream(path);
	if (stream.is_open()) {
		std::string line;
	
		while (std::getline(stream, line)) {
			fileText.append(line+"\n");
		}
	}
	stream.close();
	return fileText;
}

bool FileSystem::SaveFile(const char* path,std::string text) const
{
	bool ret = true;
	SDL_assert(path != nullptr);
	std::ofstream stream(path);
	if (stream.is_open()) {
		stream.write(text.c_str(),text.size());
	}
	stream.close();
	return ret;
}

void FileSystem::EnumerateFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs)
{
	std::string p = rootPath.string() + path;
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

void FileSystem::CreateMaterial(const char* path, const char* filename,const char* texturePath)
{
	JsonHandler jsonHandler;
	auto materialJson = R"(
{"albedo":{"color":{"a":1.0,"b":1.0,"g":1.0,"r":1.0},"texture":{"offset":{"x":0,"y":0},"path":"","scale":{"x":1,"y":1}}},"ao":{"texture":{"offset":{"x":0,"y":0},"path":"","scale":{"x":1,"y":1}},"value":0},"metallic":{"texture":{"offset":{"x":0,"y":0},"path":"","scale":{"x":1,"y":1}},"value":0},"roughness":{"texture":{"offset":{"x":0,"y":0},"path":"","scale":{"x":1,"y":1}},"value":0}}
)"_json;
	materialJson["name"] = filename;
	materialJson["albedo"]["texture"]["path"] = texturePath;
    jsonHandler.SaveJson(materialJson,path);
}
