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
class PanelCameraViewport;
class PanelGame;
class PanelRuntimeState;
class PanelAssets;
class PanelNodeEditor;
class PanelTextEditor;
class PanelSettings;
class PanelNavigation;
class PanelResources;

class GameObject;
struct EngineConfig;

struct PanelGameObjectInfo
{
	int selectedGameObjectID = 0;
	std::vector<int> selectedGameObjects;
};

struct PanelsState
{
	bool showViewportWindow = true;
	bool showCameraViewportWindow = true;
	bool showGameWindow = true;
};

class M_Editor : public Module
{
public:
	M_Editor(KoFiEngine* engine);
	~M_Editor();

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	void PrepareRender();
	void EndRender();
	void MakeCurrent();
	bool CleanUp();
	// Method to receive and manage events
	void OnNotify(const Event& event);
	void OnPlay();

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------

	void AddPanel(Panel* panel);
	void RemovePanel(Panel* panel);
	PanelChooser* GetPanelChooser();
	PanelHierarchy* GetPanelHierarchy() { return panelHierarchy; };
	PanelAssets* GetPanelAssets() { return panelAssets; }
	void Markdown(const std::string& markdown_);
	void MarkdownExample();
	void UpdatePanelsState();
	void OpenTextEditor(std::string path,const char* ext = nullptr);

	bool MouseOnScene();

	std::list<Panel*> GetPanels();
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
	ImVec2 lastCameraViewportSize;
	ImVec2 viewportSize;
	ImVec2 cameraViewportSize;
	ImVec2 scenePanelOrigin;
	ImVec2 mouseScenePosition;

	// Open/Close panel bools
	bool toggleAboutPanel = false;
	bool toggleChooserPanel = false;
	bool toggleTextEditor = false;
	bool toggleSettingsPanel = false;
	bool toggleCameraViewportPanel = false;
	bool toggleResourcesPanel = false;
	bool toggleCloseAppPopUpPanel = false;														// Enable/Disable the Close App popup.
	bool buttonPressed = false;
	PanelGameObjectInfo panelGameObjectInfo = {};
	PanelInspector* panelGameObject = nullptr;
	KoFiEngine* engine = nullptr;

	int idTracker = 0;

	bool contr = false;
	std::string iniToLoad = "";
	std::string iniToSave = "";

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
	PanelGame* panelGame = nullptr;
	PanelViewport* panelViewport = nullptr;
	PanelCameraViewport* panelCameraViewport = nullptr;
	PanelRuntimeState* panelRuntimeState = nullptr;
	PanelAssets* panelAssets = nullptr;
	PanelNodeEditor* panelNodeEditor = nullptr;
	PanelTextEditor* panelTextEditor = nullptr;
	PanelSettings* panelSettings = nullptr;
	PanelNavigation* panelNavigation = nullptr;
	PanelResources* panelResources = nullptr;
	
	bool isSceneWindowClicked = false;

};

#endif // !__EDITOR_H__