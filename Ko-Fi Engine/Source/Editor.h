#pragma once
#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H
#include "EditorStyleHandler.h"
#include "Module.h"
#include "Globals.h"
#include "Panel.h"
#include "imgui.h"

// Panels
class MainBar;
class PanelHierarchy;
class PanelConfiguration;
class PanelLog;
class PanelAbout;
class PanelChooser;
class PanelInspector;
class PanelViewport;
class PanelGame;
class PanelRuntimeState;
class PanelAssets;

class GameObject;

struct EngineConfig;

struct PanelGameObjectInfo
{
	int selectedGameObjectID = -1;
};

struct PanelsState
{
	bool showViewportWindow = true;
	bool showGameWindow = true;
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
	void RemovePanel(Panel* panel);
	PanelChooser* GetPanelChooser();
	void Markdown(const std::string& markdown_);
	void MarkdownExample();
	void UpdatePanelsState();

public:
	PanelsState panelsState;
	ImVec2 lastViewportSize;
	ImVec2 scenePanelOrigin;
	ImVec2 mouseScenePosition;

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
	std::list<Panel*> panels;

	MainBar* mainMenuBar = nullptr;
	PanelHierarchy* panelHierarchy = nullptr;
	PanelConfiguration* panelConfig = nullptr;
	PanelLog* panelLog = nullptr;
	PanelAbout* panelAbout = nullptr;
	PanelChooser* panelChooser = nullptr;
	PanelInspector* panelGameObject = nullptr;
	PanelGame* panelGame = nullptr;
	PanelViewport* panelViewport = nullptr;
	PanelRuntimeState* panelRuntimeState = nullptr;
	PanelAssets* panelAssets = nullptr;
};

#endif IM_GUI_HANDLER_H