#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"
#include "Globals.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window(KoFiEngine* engine);

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(Json configModule);

	// Called before quitting
	bool CleanUp();

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override; 
	// ------------------------------------------------------------------

	// Method to receive and manage events
	void OnNotify(const Event& event);

	// Change title
	void SetTitle(const char* title);

	void AdjustBrightness(float brightness);
	float GetBrightness();

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;
	int GetWidth() const;
	int GetHeight() const;
	bool GetFullscreen() const;
	bool GetFullscreenDesktop() const;
	bool GetResizable() const;
	bool GetBorderless() const;
	uint GetRefreshRate() const;
	const char* GetIcon() const;
	const char* GetTitle() const;
	void SetFullscreen(bool fullscreen);
	void SetFullscreenDesktop(bool fullscreenDesktop);
	void SetResizable(bool resizable);
	void SetBorderless(bool borderless);
	void SetWidth(int width);
	void SetHeight(int height);
	void SetIcon(const char* file);

	// Retrieve window scale
	uint GetScale() const;
	void GetPosition(int& x, int& y);

public:
	// The window we'll be rendering to
	SDL_Window* window;

	// The surface contained by the window
	SDL_Surface* screenSurface;

private:
	std::string title;
	uint width = SCREEN_WIDTH;
	uint height = SCREEN_HEIGHT;
	uint scale;
	float brightness;
	bool fullscreen = false;
	bool fullscreenDesktop = false;
	bool borderless = false;
	bool resizable = false;
	std::string iconFile;

	KoFiEngine* engine = nullptr;
};

#endif // !__WINDOW_H__