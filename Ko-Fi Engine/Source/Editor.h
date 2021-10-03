#pragma once
#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H

#include "Module.h"
#include "Globals.h"
#include "EditorStyleHandler.h"
#include "Panel.h"
class Window;
class Renderer3D;

// Panels
class PanelTest;
class PanelConfiguration;
class PanelLog;
struct EngineConfig;

class Editor : public Module
{
public:

	Editor(Window* window, Renderer3D* renderer,EngineConfig* engineConfig);
	~Editor();

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool CallMainMenuBar();
	void AddPanel(Panel* panel);
private:

	PanelTest* panelTest = nullptr;
	PanelConfiguration* panelConfig = nullptr;
	PanelLog* panelLog = nullptr;

	// Needed modules
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	KoFiEngine* engine = nullptr;

	EditorStyleHandler styleHandler;
	std::list<Panel*> panels;

};

#endif IM_GUI_HANDLER_H