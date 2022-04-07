#include "Engine.h"

#include "Window.h"
#include "Input.h"
#include "Renderer3D.h"
#include "M_Camera3D.h"
#include "SceneManager.h"
#include "Editor.h"
#include "FileSystem.h"
#include "FSDefs.h"
#include "UI.h"
#include "Importer.h"
#include "Globals.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "Physics.h"
#include "ResourceManager.h"
#include "M_Audio.h"
#include "Navigation.h"

#include <iostream>
#include <sstream>

#include "glew.h"

// Constructor
KoFiEngine::KoFiEngine(int argc, char* args[]) : argc(argc), args(args)
{
	engineConfig = new EngineConfig();
	PERF_START(ptimer);
	Importer::GetInstance(this);
	window = new Window(this);
	fileSystem = new FileSystem(this);
	input = new Input(this);
	camera = new M_Camera3D(this);
	renderer = new Renderer3D(this);
	editor = new Editor(this);
	sceneManager = new SceneManager(this);
	ui = new UI(this);
	//viewportBuffer = new ViewportFrameBuffer(this);
	physics = new Physics(this);
	resourceManager = new ResourceManager(this);
	audio = new M_Audio(this);
	navigation = new Navigation(this);

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(window);
	AddModule(physics);
	AddModule(input);
	AddModule(camera);
	AddModule(fileSystem);
	AddModule(ui);
	AddModule(sceneManager);
	AddModule(resourceManager);

	AddModule(navigation);
	AddModule(audio);
	AddModule(renderer);
	AddModule(editor);

	PERF_PEEK(ptimer);
}

// Destructor
KoFiEngine::~KoFiEngine()
{
	// Release modules
	for (std::list<Module*>::reverse_iterator item = modules.rbegin(); item != modules.rend(); ++item)
	{
		RELEASE(*item);
	}

	modules.clear();
}

void KoFiEngine::AddModule(Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool KoFiEngine::Awake()
{
	PERF_START(ptimer);

	//pugi::xml_document configFile;
	//pugi::xml_node config;
	//pugi::xml_node configApp;
	Json jsonConfig;
	Json jsonConfigEngine;
	bool ret = true;

	/*config = LoadConfig(configFile);*/
	ret = jsonHandler.LoadJson(jsonConfig,"EngineConfig/config.json");

	//if (config.empty() == false)
	//{
	//	ret = true;
	//	configApp = config.child("app");
	if (!jsonConfig.empty())
	{
		ret = true;
		jsonConfigEngine = jsonConfig.at("Engine");

		engineConfig->authors = jsonConfigEngine.at("Authors").get<std::string>().c_str();
		engineConfig->license = jsonConfigEngine.at("License").get<std::string>().c_str();
		engineConfig->title = jsonConfigEngine.at("Title").get<std::string>().c_str();
		engineConfig->organization = jsonConfigEngine.at("Organization").get<std::string>().c_str();
		engineConfig->maxFps = jsonConfigEngine.at("MaxFPS");
		if (engineConfig->maxFps > 0) engineConfig->cappedMs = 1000 / engineConfig->maxFps;
	}

	if (ret == true)
	{
		std::list<Module*>::iterator item = modules.begin();

		while (item != modules.end() && ret)
		{
			if (jsonConfig.contains((*item)->name))
				ret = (*item)->Awake(jsonConfig.at((*item)->name));

			item++;
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool KoFiEngine::Start()
{
	PERF_START(ptimer);

	bool ret = true;

	// Setting hardware info
	SetHardwareInfo();

	std::list<Module*>::iterator item = modules.begin();;

	while (item != modules.end() && ret)
	{
		ret = (*item)->Start();
		item++;
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool KoFiEngine::Update()
{
	bool ret = true;
	PrepareUpdate();

	/*if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;*/

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
void KoFiEngine::PrepareUpdate()
{
	engineConfig->frameCount++;
	engineConfig->lastSecFrameCount++;

	// L08: DONE 4: Calculate the dt: differential time since last frame
	engineConfig->dt = engineConfig->frameTime.ReadSec();
	engineConfig->frameTime.Start();
}

// ---------------------------------------------
void KoFiEngine::FinishUpdate()
{
	if (engineConfig->lastSecFrameTime.Read() > 1000)
	{
		engineConfig->lastSecFrameTime.Start();
		engineConfig->prevLastSecFrameCount = engineConfig->lastSecFrameCount;
		engineConfig->lastSecFrameCount = 0;
	}

	float averageFps = float(engineConfig->frameCount) / engineConfig->startupTime.ReadSec();
	float secondsSinceStartup = engineConfig->startupTime.ReadSec();
	uint32 lastFrameMs = engineConfig->frameTime.Read();
	uint32 framesOnLastUpdate = engineConfig->prevLastSecFrameCount;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %02u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %I64u ",
		averageFps, lastFrameMs, framesOnLastUpdate, engineConfig->dt, secondsSinceStartup, engineConfig->frameCount);

	engineConfig->fpsLog.push_back(engineConfig->prevLastSecFrameCount);
	if (engineConfig->fpsLog.size() > 100)
		engineConfig->fpsLog.erase(engineConfig->fpsLog.begin());

	engineConfig->msLog.push_back(lastFrameMs);
	if (engineConfig->msLog.size() > 100)
		engineConfig->msLog.erase(engineConfig->msLog.begin());

	//app->win->SetTitle(title);

	//Use SDL_Delay to make sure you get your capped framerate
	if ((engineConfig->cappedMs > 0) && (lastFrameMs < engineConfig->cappedMs))
	{
		// Measure accurately the amount of time SDL_Delay actually waits compared to what was expected
		PerfTimer pt;
		SDL_Delay(engineConfig->cappedMs - lastFrameMs);
		//CONSOLE_LOG("We waited for %d milliseconds and got back in %f", engineConfig->cappedMs - lastFrameMs, pt.ReadMs());
		//appLog->AddLog("We waited for %d milliseconds and got back in %f", engineConfig->cappedMs - lastFrameMs, pt.ReadMs());
	}
}

// Call modules before each loop iteration
bool KoFiEngine::PreUpdate()
{
	bool ret = true;

	std::list<Module*>::iterator item;
	Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; ++item)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		ret = (*item)->PreUpdate(engineConfig->dt);
	}
	SetVramStats();
	return ret;
}

// Call modules on each loop iteration
bool KoFiEngine::DoUpdate()
{
	bool ret = true;
	std::list<Module*>::iterator item = modules.begin();
	Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; ++item)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		// L08: DONE 5: Send dt as an argument to all updates, you need
		// to update module parent class and all modules that use update
		ret = (*item)->Update(engineConfig->dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool KoFiEngine::PostUpdate()
{
	bool ret = true;
	std::list<Module*>::iterator item;
	Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; ++item)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		ret = (*item)->PostUpdate(engineConfig->dt);
	}

	return ret;
}

// Called before quitting
bool KoFiEngine::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::reverse_iterator item = modules.rbegin(); item != modules.rend() && ret == true; ++item)
	{
		ret = (*item)->CleanUp();
	}

	return ret;
}

// ---------------------------------------
int KoFiEngine::GetArgc() const
{
	return argc;
}

bool KoFiEngine::SaveConfiguration() const
{
	bool ret = true;
	JsonHandler jsonHandler;
	Json jsonConfig;
	Json configEngine;
	configEngine["Authors"] = engineConfig->authors;
	configEngine["License"] = engineConfig->license;
	configEngine["Organization"] = engineConfig->organization;
	configEngine["Title"] = engineConfig->title;
	configEngine["MaxFPS"] = engineConfig->maxFps;
	jsonConfig["Engine"]=configEngine;

	for (Module* module : modules)
	{
		Json configModule;
		module->SaveConfiguration(configModule);
		jsonConfig[module->name.c_str()] = configModule;
	}


	jsonHandler.SaveJson(jsonConfig, "EngineConfig/config.json");

	return ret;
}

// ---------------------------------------
const char* KoFiEngine::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* KoFiEngine::GetTitle() const
{
	return engineConfig->title.c_str();
}

// ---------------------------------------
const char* KoFiEngine::GetOrganization() const
{
	return engineConfig->organization.c_str();
}

const uint64 KoFiEngine::GetFps() const
{
	return engineConfig->frameCount;
}

void KoFiEngine::SetHardwareInfo() {
	SDL_GetVersion(&engineConfig->sdlVersion);
	engineConfig->cpuCores = SDL_GetCPUCount();
	engineConfig->RAM = (float)(SDL_GetSystemRAM()*1024);
	engineConfig->gpuVendor = (unsigned char*)glGetString(GL_VENDOR);
	engineConfig->gpuRenderer = (unsigned char*)glGetString(GL_RENDERER);
	engineConfig->gpuVersion = (unsigned char*)glGetString(GL_VERSION);

	engineConfig->has3DNow = SDL_Has3DNow() ? true : false;
	engineConfig->hasAVX = SDL_HasAVX() ? true : false;
	engineConfig->hasAVX2 = SDL_HasAVX2() ? true : false;
	engineConfig->hasAltiVec = SDL_HasAltiVec() ? true : false;
	engineConfig->hasMMX = SDL_HasMMX() ? true : false;
	engineConfig->hasRDTSC = SDL_HasRDTSC() ? true : false;
	engineConfig->hasSSE = SDL_HasSSE() ? true : false;
	engineConfig->hasSSE2 = SDL_HasSSE2() ? true : false;
	engineConfig->hasSSE3 = SDL_HasSSE3() ? true : false;
	engineConfig->hasSSE41 = SDL_HasSSE41() ? true : false;
	engineConfig->hasSSE42 = SDL_HasSSE42() ? true : false;
}

void KoFiEngine::SetVramStats()
{
	GLint nTotalMemoryInKb = 0;
	int temp;

	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &temp); // Total Available Memory in KB
	engineConfig->vramAvailable = (float)temp;

	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &temp); // Dedicated VRAM in KB
	engineConfig->vramUsage = (float)temp;

	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &temp); // Total VRAM Memory in KB
	engineConfig->vramBudget = (float)temp;

	//TODO: GET RESERVED VRAM
}

EngineConfig* KoFiEngine::GetEngineConfig()
{
	return this->engineConfig;
}

Window* KoFiEngine::GetWindow() const
{
	return this->window;
}

Input* KoFiEngine::GetInput()const
{
	return this->input;
}

SceneManager* KoFiEngine::GetSceneManager()const
{
	return this->sceneManager;
}

Renderer3D* KoFiEngine::GetRenderer()const
{
	return this->renderer;
}

M_Camera3D* KoFiEngine::GetCamera3D()const
{
	return this->camera;
}

Editor* KoFiEngine::GetEditor()const
{
	return this->editor;
}

FileSystem* KoFiEngine::GetFileSystem()const
{
	return this->fileSystem;
}

Physics* KoFiEngine::GetPhysics()const
{
	return this->physics;
}
UI* KoFiEngine::GetUI() const
{
	return this->ui;
}
CollisionDetector* KoFiEngine::GetCollisionDetector() const
{
	return this->collisionDetector;
}

ResourceManager* KoFiEngine::GetResourceManager() const
{
	return this->resourceManager;
}
Navigation* KoFiEngine::GetNavigation() const
{
	return this->navigation;
}
