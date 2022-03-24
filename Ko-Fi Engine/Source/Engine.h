#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Module.h"
#include "EngineConfig.h"
#include "JsonHandler.h"
#include "Iterable.h"
#include <list>

// Modules
class Window;
class Input;
class SceneManager;
class Renderer3D;
class Camera3D;
class Editor;
class FileSystem;
class Physics;
class UI;
class CollisionDetector;
class ResourceManager;
class Audio;
class Navigation;

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
	Window* GetWindow() const;
	Input* GetInput() const;
	SceneManager* GetSceneManager() const;
	Renderer3D* GetRenderer() const;
	Camera3D* GetCamera3D() const;
	Editor* GetEditor() const;
	FileSystem* GetFileSystem() const;
	Physics* GetPhysics() const;
	UI* GetUI() const;
	CollisionDetector* GetCollisionDetector() const;
	ResourceManager* GetResourceManager() const;
	Navigation* GetNavigation() const;

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
	Window* window = nullptr;
	Input* input = nullptr;
	SceneManager* sceneManager = nullptr;
	Renderer3D* renderer = nullptr;
	Camera3D* camera = nullptr;
	Editor* editor = nullptr;
	FileSystem* fileSystem = nullptr;
	Physics* physics = nullptr;
	UI* ui = nullptr;
	CollisionDetector* collisionDetector = nullptr;
	ResourceManager* resourceManager = nullptr;
	Audio* audio = nullptr;
	Navigation* navigation = nullptr;

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

#endif // !__ENGINE_H__