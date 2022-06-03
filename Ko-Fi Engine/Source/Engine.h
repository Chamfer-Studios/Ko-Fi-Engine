#pragma once

#include "Module.h"
#include "EngineConfig.h"
#include "JsonHandler.h"
#include "Iterable.h"
#include <list>

// Modules
class M_Window;
class M_Input;
class M_SceneManager;
class M_Renderer3D;
class M_Camera3D;
class M_Editor;
class M_FileSystem;
class M_Physics;
class M_UI;
class CollisionDetector;
class M_ResourceManager;
class M_Audio;
class M_Navigation;

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

	// Engine config serialization
	bool SaveConfiguration() const;
	//bool LoadConfiguration(Json configModule); // It is done in the awake's function

	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	const uint64 GetFps() const;
	EngineConfig* GetEngineConfig();
	// Getters for the modules
	M_Window* GetWindow() const;
	M_Input* GetInput() const;
	M_SceneManager* GetSceneManager() const;
	M_Renderer3D* GetRenderer() const;
	M_Camera3D* GetCamera3D() const;
	M_Editor* GetEditor() const;
	M_FileSystem* GetFileSystem() const;
	M_Physics* GetPhysics() const;
	M_UI* GetUI() const;
	CollisionDetector* GetCollisionDetector() const;
	M_ResourceManager* GetResourceManager() const;
	M_Navigation* GetNavigation() const;
	M_Audio* GetAudio() const;

	double GetEngineTime() { return ptimer.ReadMs(); };

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
	// Modules
	M_Window* window = nullptr;
	M_Input* input = nullptr;
	M_SceneManager* sceneManager = nullptr;
	M_Renderer3D* renderer = nullptr;
	M_Camera3D* camera = nullptr;
	M_Editor* editor = nullptr;
	M_FileSystem* fileSystem = nullptr;
	M_Physics* physics = nullptr;
	M_UI* ui = nullptr;
	CollisionDetector* collisionDetector = nullptr;
	M_ResourceManager* resourceManager = nullptr;
	M_Audio* audio = nullptr;
	M_Navigation* navigation = nullptr;

private:
	int argc;
	char** args;
	
	PerfTimer ptimer;

	std::list<Module*> modules;
	EngineConfig* engineConfig;

	JsonHandler jsonHandler;

public:
	auto AllModules() -> decltype(make_iterable(modules.begin(), modules.end()))
	{
		return make_iterable(modules.begin(), modules.end());
	}
};