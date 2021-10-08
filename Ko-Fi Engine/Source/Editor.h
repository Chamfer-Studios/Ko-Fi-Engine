#pragma once
#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H

#include "Module.h"
#include "Globals.h"
#include "EditorStyleHandler.h"
#include "Panel.h"

// Modules
class Window;
class Renderer3D;
class Input;

// Panels
class PanelTest;
class PanelConfiguration;
class PanelLog;
class PanelAbout;

struct EngineConfig;

class Editor : public Module
{
public:

	Editor(Window* window, Renderer3D* renderer,Input* input,EngineConfig* engineConfig);
	~Editor();

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool CallMainMenuBar();
	void AddPanel(Panel* panel);

	void Markdown(const std::string& markdown_);
	void MarkdownExample();

	bool buttonPressed = false;

private:

	// Panels
	PanelTest* panelTest = nullptr;
	PanelConfiguration* panelConfig = nullptr;
	PanelLog* panelLog = nullptr;
	PanelAbout* panelAbout = nullptr;

	// Needed modules
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	KoFiEngine* engine = nullptr;

	EditorStyleHandler styleHandler;
	std::list<Panel*> panels;
};

#endif IM_GUI_HANDLER_H