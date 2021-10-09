#pragma once
#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H
#include "EditorStyleHandler.h"
#include "Module.h"
#include "Globals.h"
#include "Panel.h"

// Modules
class Window;
class Renderer3D;
class Input;

// Panels
class MainBar;
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
	void AddPanel(Panel* panel);

	void Markdown(const std::string& markdown_);
	void MarkdownExample();
public:
	bool buttonPressed = false;

private:
	//Style
	EditorStyleHandler styleHandler;
	// Panels
	MainBar* mainMenuBar = nullptr;
	PanelTest* panelTest = nullptr;
	PanelConfiguration* panelConfig = nullptr;
	PanelLog* panelLog = nullptr;
	PanelAbout* panelAbout = nullptr;

	// Needed modules
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	KoFiEngine* engine = nullptr;

	std::list<Panel*> panels;
public:
	//Open/Close panel bools
	bool toggleAboutPanel = false;
};

#endif IM_GUI_HANDLER_H