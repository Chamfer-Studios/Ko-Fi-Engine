#pragma once
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Module.h"
#include "EngineConfig.h"
#include <list>
#include "JsonHandler.h"

// Modules

class Window;
class Input;

class SceneIntro;
class Renderer3D;
class Camera3D;
class Editor;

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
	const uint64 GetFps() const;

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

	void SetHardwareInfo();
	void SetVramStats();

private:

	//Modules
	Window* window = nullptr;
	Input* input = nullptr;
	SceneIntro* sceneIntro = nullptr;
	Renderer3D* renderer = nullptr;
	Camera3D* camera = nullptr;
	Editor* editor = nullptr;

private:

	int argc;
	char** args;
	
	PerfTimer ptimer;

	std::list<Module*> modules;
	EngineConfig* engineConfig;

	JsonHandler jsonHandler;
};

#endif //__ENGINE_H__