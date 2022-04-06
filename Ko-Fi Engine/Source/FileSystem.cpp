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

bool FileSystem::Awake(Json configModule)
{
	bool ret = true;

	CONSOLE_LOG("Turning on FileSystem debugger...");
	appLog->AddLog("Turning on FileSystem debugger...\n");

	//Prepare filesystem
	//std::filesystem::directory_entry().assign(addPath);
	// Stream log messages to Debug window

	ret = LoadConfiguration(configModule);

	return ret;
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

// Method to receive and manage events
void FileSystem::OnNotify(const Event& event)
{
	// Manage events
}

bool FileSystem::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool FileSystem::LoadConfiguration(Json& configModule)
{
	return true;
}

bool FileSystem::InspectorDraw()
{
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
			fileText.append(line + "\n");
		}
	}
	stream.close();
	return fileText;
}

std::string FileSystem::OpenFileBinary(const char* path) const
{
	std::string fileText;

	SDL_assert(path != nullptr);
	std::ifstream stream(path, std::ios::binary);
	if (stream.is_open()) {
		std::string line;

		while (std::getline(stream, line)) {
			fileText.append(line + "\n");
		}
	}
	stream.close();
	return fileText;
}

bool FileSystem::SaveFile(const char* path, std::string text) const
{
	bool ret = true;
	SDL_assert(path != nullptr);
	std::ofstream stream(path);
	if (stream.is_open()) {
		stream.write(text.c_str(), text.size());
	}
	stream.close();
	return ret;
}

void FileSystem::EnumerateFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs)
{
	std::string p = rootPath.string() + "/" + path;
	for (const auto& file : std::filesystem::directory_iterator(p))
	{
		if (std::filesystem::is_directory(file.path()))
		{
			dirs.push_back(file.path().filename().string());
		}
		else
		{
			files.push_back(file.path().filename().string());
		}
	}
}

void FileSystem::DiscoverAllFiles(const char* directory, std::vector<std::string>& files)
{
	if (directory == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: discovering files, directory string was nullptr.");
		return;
	}
	if (!std::filesystem::exists(directory))
	{
		CONSOLE_LOG("[ERROR] Resource Manager: discovering files, directory %s doesn't exist or not found.", directory);
		return;
	}

	std::vector<std::string> directories;
	EnumerateFiles(directory, files, directories);

	for (const auto dir : directories)
	{
		std::string path = directory + std::string("/") + dir.c_str();
		DiscoverAllFiles(path.c_str(), files);
	}

	directories.clear();
	directories.shrink_to_fit();
}

void FileSystem::DiscoverAllFilesFiltered(const char* directory, std::vector<std::string>& files, std::vector<std::string>& filteredFiles, const char* filter)
{
	if (directory == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: discovering files, directory string was nullptr.");
		return;
	}
	if (!std::filesystem::exists(directory))
	{
		CONSOLE_LOG("[ERROR] Resource Manager: discovering files, directory %s doesn't exist or not found.", directory);
		return;
	}
	if (filter == nullptr)
	{
		CONSOLE_LOG("[ERROR] Resource Manager: Filter was nullptr.");
		return;
	}

	std::vector<std::string> directories;
	EnumerateFiles(directory, files, directories);

	for (auto & file : files)
	{
		std::filesystem::path fileTmp = file;
		if (!fileTmp.extension().empty())
		{
			if (StringCompare((const char*)fileTmp.extension().c_str(), filter) == 0)
				files.push_back(file);
		}
	}

	for (const auto dir : directories)
	{
		std::string path = directory + std::string("/") + dir.c_str();
		DiscoverAllFilesFiltered(path.c_str(), files, filteredFiles, filter);
	}

	directories.clear();
	directories.shrink_to_fit();
}

void FileSystem::GetLastModTime(const char* path)
{
	auto fTime = std::filesystem::last_write_time(path);
}

const char* FileSystem::GetFileName(const char* path) const
{
	std::string p = path;
	std::string name = p.substr(p.find_last_of("/") + 1, p.size());
	const char* n = name.c_str();
	return n;
}

std::string FileSystem::GetNameFromPath(std::string path)
{
	std::string name = path;
	name = name.substr(name.find_last_of("/\\") + 1);
	std::string::size_type const p(name.find_last_of('.'));
	name = name.substr(0, p);
	return name;
}

int FileSystem::StringCompare(const char* a, const char* b) {
	int ca, cb;
	do
	{
		ca = *(unsigned char*)a;
		cb = *(unsigned char*)b;
		ca = tolower(toupper(ca));
		cb = tolower(toupper(cb));
		a++;
		b++;
	} while (ca == cb && ca != '\0');
	return ca - cb;
}

void FileSystem::AddPath(const char* path)
{
	rootPath += path;
}

void FileSystem::CreateMaterial(const char* path, const char* filename, const char* texturePath)
{
	JsonHandler jsonHandler;
	auto materialJson = R"(
	{
		"name" : "",
		"path" : "",
		"textures" : [""],
		"uniforms" :
		{
			"albedoTint" :
			{
				"x":1.0,
				"y":1.0,
				"z":1.0,
				"w":1.0
			}
		}
	}
	)"_json;

	materialJson["name"] = filename;

	auto texturesArray = json::array();
	texturesArray.push_back(texturePath);

	materialJson["textures"] = texturesArray;

	jsonHandler.SaveJson(materialJson, path);
}

void FileSystem::CreateMaterial(const char* path)
{
	JsonHandler jsonHandler;
	auto materialJson = R"(
	{
		"name" : "",
		"path" : "",
		"textures" : [""],
		"uniforms" : 
		{
			"albedoTint" :
			{
				"x":1.0,
				"y":1.0,
				"z":1.0,
				"w":1.0
			}
		}
	}
	)"_json;

	materialJson["name"] = "default";
	materialJson["path"] = path;

	jsonHandler.SaveJson(materialJson, path);
}

void FileSystem::CreateShader(const char* path)
{
	const char* text = R"(
	#shader vertex
	#version 330 core

	layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 normals;
	layout (location = 2) in vec2 texCoord;

	out vec4 ourColor;
	out vec2 TexCoord;

	uniform mat4 model_matrix;
	uniform mat4 view;
	uniform mat4 projection;
	uniform vec4 albedoTint;

	void main()
	{
		gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
		ourColor = albedoTint;
		TexCoord = texCoord;
	}

	#shader fragment
	#version 330 core

	in vec4 ourColor;
	in vec2 TexCoord;

	out vec4 color;

	uniform sampler2D ourTexture;

	void main()
	{
		color = texture(ourTexture, TexCoord)*ourColor;
	}
	)";

	SDL_assert(path != nullptr);

	std::ofstream stream(path, std::ios::trunc);
	SDL_assert(stream.is_open());
	try
	{
		stream << std::setw(4) << text << std::endl;

	}
	catch (Json::parse_error& e)
	{
		CONSOLE_LOG("Error while Saving File: %c", e.what());
		appLog->AddLog("Error while Saving File: %c\n", e.what());
	}
	stream.close();
}

void FileSystem::CreateScene(const char* path,const char* sceneName)
{
	Json sceneJson;
	std::string name = GetNameFromPath(sceneName);
	sceneJson[name];
	sceneJson[name]["active"] = true;
	sceneJson[name]["game_objects_amount"] = 0;
	sceneJson[name]["game_objects_list"] = json::array();
	sceneJson[name]["name"] = name;

	SDL_assert(path != nullptr);

	JsonHandler handler;
	handler.SaveJson(sceneJson, path);
}
