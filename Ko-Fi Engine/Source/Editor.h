#pragma once
#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H
#include "EditorStyleHandler.h"
#include "Module.h"
#include "Globals.h"
#include "Panel.h"

// Panels
class MainBar;
class PanelHierarchy;
class PanelConfiguration;
class PanelLog;
class PanelAbout;
class PanelChooser;
class PanelInspector;

struct EngineConfig;
class GameObject;

struct PanelGameObjectInfo
{
	int currentGameObjectID = -1;
};

class Editor : public Module
{
public:

	Editor(KoFiEngine* engine);
	~Editor();

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	void AddPanel(Panel* panel);
	PanelChooser* GetPanelChooser();
	void Markdown(const std::string& markdown_);
	void MarkdownExample();

public:
	// Open/Close panel bools
	bool toggleAboutPanel = false;
	bool toggleChooserPanel = false;
	bool buttonPressed = false;
	PanelGameObjectInfo panelGameObjectInfo = {};
	KoFiEngine* engine = nullptr;

private:
	// Style
	EditorStyleHandler styleHandler;
	// Panels
	MainBar* mainMenuBar = nullptr;
	PanelHierarchy* panelHierarchy = nullptr;
	PanelConfiguration* panelConfig = nullptr;
	PanelLog* panelLog = nullptr;
	PanelAbout* panelAbout = nullptr;
	PanelChooser* panelChooser = nullptr;
	PanelInspector* panelGameObject = nullptr;
private:
	std::list<Panel*> panels;
};

#endif IM_GUI_HANDLER_H