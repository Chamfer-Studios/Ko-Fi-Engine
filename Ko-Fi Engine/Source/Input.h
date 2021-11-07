#pragma once
#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5


// FIXME: The list of meshes should be in scene intro.
class GameObject;

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Input : public Module
{
public:

	Input(KoFiEngine* engine);
	~Input();

	bool Init();
	bool PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

public:

private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	//int mouse_z_motion;
	//Renderer3D* renderer = nullptr;
	char* dropped_filedir;
	KoFiEngine* engine = nullptr;
};