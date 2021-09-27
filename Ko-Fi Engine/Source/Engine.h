#pragma once
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"

#include <list>

// Modules
//class FileSystem;
class Window;
class Input;
//class SceneManager;
class SceneIntro;
class Renderer3D;
class Camera3D;
//class Editor;
//class Resources;
//class Shaders;
//
//class GameObject;
class ImGuiHandler;

class KoFiEngine
{
public:

	// Constructor
	KoFiEngine(int argc, char* args[]);

	// Destructor
	virtual ~KoFiEngine();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Exposing some properties for reading
	int GetArgc() const;

	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

private:
	// Add a new module to handle
	void AddModule(Module* module);

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

public:
	//Modules
	/*FileSystem* fileSystem = nullptr;*/
	Window* window = nullptr;
	Input* input = nullptr;
	//SceneManager* sceneManager = nullptr;
	SceneIntro* sceneIntro = nullptr;
	Renderer3D* renderer = nullptr;
	Camera3D* camera = nullptr;
	//Editor* editor = nullptr;
	//Resources* resources = nullptr;
	//Shaders* shaders = nullptr;
	ImGuiHandler* imGUIHandler = nullptr;

private:
	int argc;
	char** args;
	SString title;
	SString organization;

	std::list<Module*> modules;

	PerfTimer ptimer;
	uint64 frameCount = 0;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	float dt = 0.0f;

	int	cappedMs = -1;
};

#endif //__ENGINE_H__