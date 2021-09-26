#pragma once
#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H

#include "Module.h"
#include "Globals.h"
#include "ImGuiStyleHandler.h"

class Window;
class Renderer3D;

class ImGuiHandler : public Module
{
public:
	ImGuiHandler(Window* window, Renderer3D* renderer);
	~ImGuiHandler();

	bool Awake();
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void CreateWin(SString name, SString text,float widht = 250.0f,float height=100.0f);
	void CreateButton();

	bool buttonPressed = false;

private:
	// Needed modules
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	ImGuiStyleHandler styleHandler;

	bool newWindow = false;
};

#endif IM_GUI_HANDLER_H