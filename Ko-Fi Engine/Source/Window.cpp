#include "Window.h"
#include "Engine.h"
#include "Defs.h"
#include "Log.h"

#include "SDL.h"


Window::Window() : Module()
{
	window = NULL;
	screenSurface = NULL;
	name.Create("Window");
}

// Destructor
Window::~Window()
{
}

// Called before render is available
bool Window::Awake(Json configModule)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		// Create window
		// L01: DONE 6: Load all required configurations from config.xml
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		// Set up SDL to use OpenGL 2.x
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
		bool fullscreen = false;
		bool borderless = false;
		bool resizable = false;
		bool fullscreen_window = false;

		width = 1024;
		height = 720;
		scale = 1;

		if(fullscreen == true) flags |= SDL_WINDOW_FULLSCREEN;
		if(borderless == true) flags |= SDL_WINDOW_BORDERLESS;
		if(resizable == true) flags |= SDL_WINDOW_RESIZABLE;
		if(fullscreen_window == true) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			SDL_assert(window != NULL);
			ret = false;
		}
		else
		{
			// Get window surface
			screenSurface = SDL_GetWindowSurface(window);
			//Get window brightness
			brightness = SDL_GetWindowBrightness(window);
		}
	}

	return ret;
}

// Called before quitting
bool Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	// Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	// Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void Window::SetTitle(const char* new_title)
{
	//title.create(new_title);
	SDL_SetWindowTitle(window, new_title);
}

void Window::AdjustBrightness(float brightness)
{
	brightness = brightness < 0 ? 0 : brightness;
	brightness = brightness > 1 ? 1 : brightness;

	SDL_SetWindowBrightness(window, brightness);
	this->brightness = SDL_GetWindowBrightness(window);
}

float Window::GetBrightness()
{
	return this->brightness;
}

void Window::GetWindowSize(uint& width, uint& height) const
{
	width = this->width;
	height = this->height;
}
int Window::GetWidth() const
{
	return (int)this->width;
}
int Window::GetHeight() const
{
	return (int)this->height;
}

void Window::SetWidth(int width)
{
	SDL_assert(width >= 0);
	this->width = (uint)width;
}

void Window::SetHeight(int height)
{
	SDL_assert(height >= 0);
	this->height = (uint)height;
}

uint Window::GetScale() const
{
	return scale;
}