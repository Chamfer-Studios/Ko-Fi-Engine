#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(Json configModule);

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* title);

	void AdjustBrightness(float brightness);
	float GetBrightness();

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;
	int GetWidth() const;
	int GetHeight() const;
	void SetWidth(int width);
	void SetHeight(int height);

	// Retrieve window scale
	uint GetScale() const;

public:
	// The window we'll be rendering to
	SDL_Window* window;

	// The surface contained by the window
	SDL_Surface* screenSurface;

private:
	SString title;
	uint width;
	uint height;
	uint scale;
	float brightness;
};

#endif // __WINDOW_H__