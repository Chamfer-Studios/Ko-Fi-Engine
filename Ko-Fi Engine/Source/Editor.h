#pragma once
#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H

#include "Module.h"
#include "Globals.h"
#include "EditorStyleHandler.h"
#include "Panel.h"
class Window;
class Renderer3D;

//Panels
class PanelTest;
class PanelConfiguration;
class Editor : public Module
{
public:
	Editor(Window* window, Renderer3D* renderer);
	~Editor();

	bool Awake();
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	void AddPanel(Panel* panel);
	bool buttonPressed = false;

private:
	PanelTest* panelTest = nullptr;
	PanelConfiguration* panelConfig = nullptr;
private:
	// Needed modules
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	KoFiEngine* engine = nullptr;
	EditorStyleHandler styleHandler;
	std::list<Panel*> panels;

	bool newWindow = false;
};

#endif IM_GUI_HANDLER_H