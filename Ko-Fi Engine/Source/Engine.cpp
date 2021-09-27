#include "Engine.h"

#include "Window.h"
#include "Input.h"
#include "Renderer3D.h"
#include "Camera3D.h"
#include "SceneIntro.h"
//#include "Textures.h"
//#include "Audio.h"
//#include "EntityManager.h"
//#include "SceneManager.h"
#include "ImGUIHandler.h"

#include "Defs.h"
#include "Log.h"

#include <iostream>
#include <sstream>

// Constructor
KoFiEngine::KoFiEngine(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	window = new Window();
	input = new Input();
	camera = new Camera3D(input);
	renderer = new Renderer3D(window,camera);
	imGUIHandler = new ImGuiHandler(window, renderer);
	sceneIntro = new SceneIntro(camera, window, renderer, imGUIHandler);
	//tex = new Textures(render);
	//audio = new AudioManager();
	//entityManager = new EntityManager();
	//sceneManager = new SceneManager(input, render, tex);

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(imGUIHandler);
	AddModule(sceneIntro);
	//AddModule(tex);
	//AddModule(audio);
	//AddModule(entityManager);
	//AddModule(sceneManager);

	// Render last to swap buffer
	AddModule(renderer);

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
	//TODO: CHANGE TO FALSE AFTER CONFIG LOAD FROM JSON
	bool ret = true;

	/*config = LoadConfig(configFile);*/

	//if (config.empty() == false)
	//{
	//	ret = true;
	//	configApp = config.child("app");

	if (ret == true)
	{
		std::list<Module*>::iterator item = modules.begin();;

		while ((*item != NULL) && (ret == true))
		{
			ret = (*item)->Awake();
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
	std::list<Module*>::iterator item = modules.begin();;

	while (*item != NULL && ret == true)
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

// Load config from XML file
// NOTE: Function has been redesigned to avoid storing additional variables on the class
//pugi::xml_node App::LoadConfig(pugi::xml_document& configFile) const
//{
//	pugi::xml_node ret;
//
//	pugi::xml_parse_result result = configFile.load_file(CONFIG_FILENAME);
//
//	if (result == NULL) LOG("Could not load xml file: %s. pugi error: %s", CONFIG_FILENAME, result.description());
//	else ret = configFile.child("config");
//
//	return ret;
//}

// ---------------------------------------------
void KoFiEngine::PrepareUpdate()
{
	frameCount++;
	lastSecFrameCount++;

	// L08: DONE 4: Calculate the dt: differential time since last frame
	dt = frameTime.ReadSec();
	frameTime.Start();
}

// ---------------------------------------------
void KoFiEngine::FinishUpdate()
{
	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		prevLastSecFrameCount = lastSecFrameCount;
		lastSecFrameCount = 0;
	}

	float averageFps = float(frameCount) / startupTime.ReadSec();
	float secondsSinceStartup = startupTime.ReadSec();
	uint32 lastFrameMs = frameTime.Read();
	uint32 framesOnLastUpdate = prevLastSecFrameCount;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %02u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %I64u ",
		averageFps, lastFrameMs, framesOnLastUpdate, dt, secondsSinceStartup, frameCount);

	//app->win->SetTitle(title);

	// L08: DONE 2: Use SDL_Delay to make sure you get your capped framerate
	if ((cappedMs > 0) && (lastFrameMs < cappedMs))
	{
		// L08: DONE 3: Measure accurately the amount of time SDL_Delay actually waits compared to what was expected
		PerfTimer pt;
		SDL_Delay(cappedMs - lastFrameMs);
		LOG("We waited for %d milliseconds and got back in %f", cappedMs - lastFrameMs, pt.ReadMs());
	}
}

// Call modules before each loop iteration
bool KoFiEngine::PreUpdate()
{
	bool ret = true;

	std::list<Module*>::iterator item;
	Module* pModule = NULL;

	for (item = modules.begin(); *item != NULL && ret == true; ++item)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		ret = (*item)->PreUpdate(dt);
	}

	return ret;
}

// Call modules on each loop iteration
bool KoFiEngine::DoUpdate()
{
	bool ret = true;
	std::list<Module*>::iterator item = modules.begin();
	Module* pModule = NULL;

	for (item = modules.begin(); *item != NULL && ret == true; ++item)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		// L08: DONE 5: Send dt as an argument to all updates, you need
		// to update module parent class and all modules that use update
		ret = (*item)->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool KoFiEngine::PostUpdate()
{
	bool ret = true;
	std::list<Module*>::iterator item;
	Module* pModule = NULL;

	for (item = modules.begin(); *item != NULL && ret == true; ++item)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		ret = (*item)->PostUpdate(dt);
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
	return title.GetString();
}

// ---------------------------------------
const char* KoFiEngine::GetOrganization() const
{
	return organization.GetString();
}