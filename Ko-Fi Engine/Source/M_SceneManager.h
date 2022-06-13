#ifndef __M_SCENE_MANAGER_H__
#define __M_SCENE_MANAGER_H__

#include "Module.h"

#include "Timer.h"
#include "MathGeoLib/Math/float2.h"

#include "ImGuizmo.h"
#include "Scene.h"

#include <string>
#include <imgui.h>
#include <imgui_internal.h>

class KoFiEngine;
class SceneIntro;
class GameObject;
class C_Transform;
class C_Mesh;
class C_Material;
class C_Info;
class C_Camera;
class C_Canvas;
class C_Transform2D;
class C_Text;
class C_Image;
class C_Button;

enum class GameState
{
	PLAYING,
	PAUSED,
	STOPPED,
	TICK
};

class M_SceneManager : public Module
{
public:
	M_SceneManager(KoFiEngine* engine);
	~M_SceneManager();

	// Called before render is available
	bool Awake(Json configModule);

	// Called before the first frame
	virtual bool Start();

	// Called each loop iteration
	virtual bool PreUpdate(float dt);

	// Called each loop iteration
	virtual bool Update(float dt);

	// Called each loop iteration
	virtual bool PostUpdate(float dt);

	// Called before quitting
	virtual bool CleanUp();

	// Method to receive and manage events
	void OnNotify(const Event& event);

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------

	bool PrepareUpdate();
	bool FinishUpdate();

	void AddScene(Scene* scene);
	Scene* GetCurrentScene();

	GameState GetGameState();
	inline float const GetGameTime() { return time; }
	inline float GetGameDt() const { return gameDt; }
	inline float GetTotalGameTime() const { return gameTime; }
	inline std::string GetDefaultScene() const { return defaultScene; }

	bool LoadResourceToScene(Resource* resource);
	bool ApplyTextureToSelectedGameObject(UID uid);
	bool CreateGameObjectsFromModel(R_Model* model);
	void CreateComponentsFromNode(R_Model* model, ModelNode node, GameObject* gameobject);

	void OnPlay();
	void OnSceneSwitch();
	void OnStop();
	void OnPause();
	void OnResume();
	void OnTick();
	void OnClick(SDL_Event event);

	void ToggleRuntime(); // Method to stop or resume the runtime.

	// Guizmo
	ImGuizmo::OPERATION GetGizmoOperation() { return currentGizmoOperation; }
	void SetGizmoOperation(ImGuizmo::OPERATION operation) { currentGizmoOperation = operation; }
	void GuizmoTransformation();
	void UpdateGuizmo();

	// Mouse Texture functions
	bool ChangeMouseTexture(std::string texturePathToBMPImage);

public:
	bool active;
	Scene* currentScene = nullptr;
	SceneIntro* sceneIntro = nullptr;

private:
	// Pointer to the whole engine
	KoFiEngine* engine = nullptr;

	// Scenes
	std::vector<Scene*> scenes;
	std::string defaultScene = "";

	// Guizmo
	ImGuizmo::OPERATION currentGizmoOperation;
	ImGuizmo::MODE currentGizmoMode;

	ImGuiWindow* window = nullptr;
	float2 cornerPos;

	GameState runtimeState = GameState::STOPPED;

	// GAME TIME MANAGEMENT
	// ----------------------------------------------------------------------------------------------------
	// Frame Count: app graphics frames since game start
	uint frameCount = 0;
	// Time: second since game start (Game Clock)
	float time = 0.0f; // To keep the seconds when we pause the simulation
	// Time Scale: scale at which time is passing (Game Clock)
	// Modify this variable to change the speed of our game in runtime.
	float timeScale = 1.0f; // 1 --> Real Time / < 1 --> Slower / > 1 --> Faster, 0 --> Pause or Stop
	// Game Delta Time: last frame time expressed in seconds (Game Clock)
	Timer timer; // Used for gameDt (resets each frame)
	float gameDt = 0.0f;
	float gameClockSpeed = 0.0f; // != 0 (timeScale) --> Playing / == 0 --> Pause or Stop
	float gameTime = 0.0f; // Total game time
	// Real Time Since Startup: seconds since game start (Real Time Clock) --> Engine.cpp
	// Real Time Delta Time: last frame time expressed in seconds (Real Time Clock) --> Engine.cpp
	// ----------------------------------------------------------------------------------------------------
	//Current Mouse Texture
	std::string currentMouseTextPath = "";
};

#endif // !__M_SCENE_MANAGER_H__