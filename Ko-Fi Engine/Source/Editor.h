#ifndef __EDITOR_H__
#define __EDITOR_H__

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
class PanelNodeEditor;
class PanelTextEditor;
class PanelSettings;
class PanelNavigation;

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
	// Method to receive and manage events
	void OnNotify(const Event& event);

	void AddPanel(Panel* panel);
	void RemovePanel(Panel* panel);
	PanelChooser* GetPanelChooser();
	void Markdown(const std::string& markdown_);
	void MarkdownExample();
	void UpdatePanelsState();
	void OpenTextEditor(std::string path,const char* ext = nullptr);

	template<class T> T* GetPanel()
	{
		T* panel = nullptr;
		for (Panel* p : panels)
		{
			panel = dynamic_cast<T*>(p);
			if (panel)
				break;
		}
		return panel;
	}

public:
	PanelsState panelsState;
	ImVec2 lastViewportSize;
	ImVec2 viewportSize;
	ImVec2 scenePanelOrigin;
	ImVec2 mouseScenePosition;

	// Open/Close panel bools
	bool toggleAboutPanel = false;
	bool toggleChooserPanel = false;
	bool toggleTextEditor = false;
	bool toggleSettingsPanel = false;
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
	PanelNodeEditor* panelNodeEditor = nullptr;
	PanelTextEditor* panelTextEditor = nullptr;
	PanelSettings* panelSettings = nullptr;
	PanelNavigation* panelNavigation = nullptr;
	
	bool isSceneWindowClicked = false;

};

#endif // !__EDITOR_H__