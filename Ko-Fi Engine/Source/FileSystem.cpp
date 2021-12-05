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

FileSystem::FileSystem(KoFiEngine* engine)
{
	name = "ModelLoader";
	std::string rootPathString = SDL_GetBasePath();
	std::replace(rootPathString.begin(), rootPathString.end(), '\\', '/');
	rootPath = rootPathString;
	rootPath = rootPath.parent_path().parent_path();
	//Comment this for release path and uncomment when developing
	AddPath("/Ko-Fi Engine/Ko-Fi");
	//Uncomment this for release path and comment when developing
	//AddPath("/Assets");
	this->engine = engine;
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

void FileSystem::GameObjectFromMesh(const char* file_path, std::vector<GameObject*>& gameObjects, const char* texturePath)
{
	//TODO: get game objects list from scene not by passing as parameter
	uint id = gameObjects.size();
	GameObject* gameObject = new GameObject(id);
	/*gameObject->CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);
	if (texturePath == nullptr)
	{
		gameObject->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH);
	}
	else {
		ComponentMesh* meshComponent = (ComponentMesh*)gameObject->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH);
		meshComponent->materialComponent->LoadTexture(texturePath);
	}
	gameObjects.push_back(gameObject);
	engine->GetSceneIntro()->rootGo->SetChild(gameObject);*/
}

//void FileSystem::GameObjectFromPrimitive(COMPONENT_SUBTYPE subtype, std::vector<GameObject*>& gameObjects)
//{
//	uint id = gameObjects.size();
//	GameObject* gameObject = new GameObject(id);
//	gameObject->CreateComponent(COMPONENT_TYPE::COMPONENT_MESH, subtype);
//	gameObjects.push_back(gameObject);
//	engine->GetSceneIntro()->rootGo->SetChild(gameObject);
//}
