#include "Globals.h"
#include "M_Input.h"
// Modules
#include "Engine.h"
#include "M_Renderer3D.h"
#include "M_SceneManager.h"
#include "M_Editor.h"
#include "M_Window.h"
#include "M_FileSystem.h"
#include "M_ResourceManager.h"

// GameObject
#include "GameObject.h"
#include "C_Material.h"

// Resources
#include "Importer.h"
#include "R_Texture.h"

#include "Log.h"
#include "ImGuiAppLog.h"
#include "FSDefs.h"
#include "SDL.h"

#include "PanelAssets.h"

#include <imgui_impl_sdl.h>

#include "optick.h"

#define MAX_KEYS 300

M_Input::M_Input(KoFiEngine* engine) : Module()
{
	name = "Input";

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
	this->engine = engine;
}

// Destructor
M_Input::~M_Input()
{
	delete[] keyboard;
}

bool M_Input::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

// Called before render is available
bool M_Input::Init()
{
	CONSOLE_LOG("Init SDL input event system");
	appLog->AddLog("Init SDL input event system\n");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		CONSOLE_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		appLog->AddLog("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	return ret;
}

// Called every draw update
bool M_Input::PreUpdate(float dt)
{
	OPTICK_EVENT();

	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_MOUSEWHEEL:
			mouse_z = event.wheel.y;
			break;

		case SDL_MOUSEMOTION:
			mouse_x = event.motion.x / SCREEN_SIZE;
			mouse_y = event.motion.y / SCREEN_SIZE;

			mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
			break;

		case SDL_MOUSEBUTTONDOWN:
			for (auto m : engine->AllModules())
			{
				m->OnClick(event);
			}
			break;

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
		{
			switch (event.window.event)
			{
				//case SDL_WINDOWEVENT_LEAVE:
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
				//SDL_MinimizeWindow(win->window);
				break;
				//case SDL_WINDOWEVENT_ENTER:
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_MAXIMIZED:
				//SDL_MaximizeWindow(win->window);
				break;
			case SDL_WINDOWEVENT_RESTORED:
				//windowEvents[WE_SHOW] = true;
				//SDL_RestoreWindow(win->window);
				break;
			}
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				engine->GetWindow()->SetWidth(event.window.data1);
				engine->GetWindow()->SetHeight(event.window.data2);
			}
			
			break;
		}
		//case (SDL_DROPFILE): {      // In case if dropped file
		//	dropped_filedir = event.drop.file;
		//	// Shows directory of dropped file
		//	SDL_ShowSimpleMessageBox(
		//		SDL_MESSAGEBOX_INFORMATION,
		//		"File dropped on window",
		//		dropped_filedir,
		//		window->window
		//	);
		//	SDL_free(dropped_filedir);    // Free dropped_filedir memory
		//	break;
		//}
		case SDL_DROPFILE:
		{
			std::vector<std::string> assetsFiles;
			std::vector<std::string> metaFiles;
			engine->GetFileSystem()->DiscoverAllFilesFiltered(ASSETS_DIR, assetsFiles, metaFiles, META_EXTENSION);

			std::filesystem::path originPath = event.drop.file;
			if (!originPath.empty())
			{
				if (engine->GetFileSystem()->StringCompare(originPath.extension().string().c_str(), FBX_EXTENSION) == 0
					|| engine->GetFileSystem()->StringCompare(originPath.extension().string().c_str(), PNG_EXTENSION) == 0)
				{
					std::string destinationDir = engine->GetEditor()->GetPanelAssets()->GetCurrentDir();
					for (uint i = 0; i < destinationDir.size(); ++i)
					{
						if (destinationDir[i] == '\\')
							destinationDir[i] = '/';
					}

					bool found = false;
					std::filesystem::path metaFilename;
					for (const auto& metaIt : metaFiles)
					{
						metaFilename = metaIt;
						std::string tmp = originPath.filename().string() + META_EXTENSION;
						if (metaFilename.filename().string() == tmp)
						{
							found = true;
							break;
						}
					}

					destinationDir += "/";
					std::string path;
					if (found)
					{
						destinationDir = metaFilename.parent_path().string();
						destinationDir += "/";

						path = destinationDir + originPath.filename().string();
						appLog->AddLog("[STATUS] The file %s has been updated.", path.c_str());
					}
					else
					{
						path = destinationDir + originPath.filename().string();
						appLog->AddLog("[STATUS] The file %s has been imported successfully.", path.c_str());
					}

					engine->GetFileSystem()->CopyFileTo(originPath.string().c_str(), path.c_str());
					engine->GetResourceManager()->RefreshDirectoryFiles(destinationDir.c_str());
				}
			}
			break;
		}
		}
	}
	ImGui_ImplSDL2_ProcessEvent(&event);

	if (quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP || quitGame == true)
		return false;

	return true;
}

// Called before quitting
bool M_Input::CleanUp()
{
	CONSOLE_LOG("Quitting SDL input event subsystem");
	appLog->AddLog("Quitting SDL input event subsystem\n");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// Method to receive and manage events
void M_Input::OnNotify(const Event& event)
{
	// Manage events
}

bool M_Input::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_Input::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_Input::InspectorDraw()
{
	return true;
}