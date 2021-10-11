#include "FileLoader.h"
#include "Log.h"
#include "ImGuiAppLog.h"

// Assimp (Open Asset Import Library)
//#include "assimp/Importer.hpp"
#include "assimp/cimport.h"     //#include "Assimp/include/cimport.h"
#include "assimp/scene.h"       //#include "Assimp/include/scene.h"
#include "assimp/postprocess.h" //#include "Assimp/include/postprocess.h"

//#pragma comment (lib, "Assimp/libx86/assimp.lib")
//Already included in the project configuration...

FileLoader::FileLoader()
{
	name = "ModelLoader";
}

FileLoader::~FileLoader()
{

}

bool FileLoader::Awake()
{
	LOG("Turning on Assimp debugger...");
	appLog->AddLog("Turning on Assimp debugger...\n");

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool FileLoader::Start()
{
	LOG("Starting Assimp...");
	appLog->AddLog("Starting Assimp...\n");

	return true;
}

bool FileLoader::PreUpdate(float dt)
{
	return true;
}

bool FileLoader::Update(float dt)
{
	return true;
}

bool FileLoader::PostUpdate(float dt)
{
	return true;
}

bool FileLoader::CleanUp()
{
	LOG("Cleaning Assimp up...");
	appLog->AddLog("Cleaning Assimp up...\n");

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}
