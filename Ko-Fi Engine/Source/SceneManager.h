#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "Module.h"
#include "Scene.h"

#include "Timer.h"
#include <string>
#include <list>

#include "ImGuizmo.h"

class KoFiEngine;
class SceneIntro;
class GameObject;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;
class ComponentInfo;
class ComponentCamera;
class ComponentCanvas;
class ComponentTransform2D;
class ComponentText;
class ComponentImage;
class ComponentButton;

enum class GameState
{
	PLAYING,
	PAUSED,
	STOPPED,
	TICK
};

class SceneManager : public Module
{
public:
	SceneManager(KoFiEngine* engine);
	~SceneManager();

	// Called before render is available
	virtual bool Awake();

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

	bool PrepareUpdate();
	bool FinishUpdate();

	void AddScene(Scene* scene);
	Scene* GetCurrentScene();

	GameState GetGameState();
	inline float const GetGameTime() { return time; }
	inline float GetGameDt() const { return gameDt; }
	inline float GetTotalGameTime() const { return gameTime; }

	void OnPlay();
	void OnStop();
	void OnPause();
	void OnResume();
	void OnTick();
	void OnClick(SDL_Event event);

	//GUIZMO
	ImGuizmo::OPERATION GetGizmoOperation() { return currentGizmoOperation; }
	void SetGizmoOperation(ImGuizmo::OPERATION operation) { currentGizmoOperation = operation; }
	void GuizmoTransformation();
	void UpdateGuizmo();
	//

	inline float GetGameDt() const { return gameDt; }
public:
	bool active;

private:
	// Modules
	KoFiEngine* engine = nullptr;

	// Scenes
	std::vector<Scene*> scenes;
	Scene* currentScene = nullptr;
	SceneIntro* sceneIntro = nullptr;

	// Guizmo
	ImGuizmo::OPERATION currentGizmoOperation;
	ImGuizmo::MODE currentGizmoMode;

	// TIME MANAGEMENT
	// --------------------------------------------------
	// Frame Count: app graphics frames since game start
	uint frameCount = 0;
	// Time: second since game start (Game Clock)
	float time = 0.0f; // To keep the seconds when we pause the simulation
	// Time Scale: scale at which time is passing (Game Clock)
	// Modify this variable to change the speed of our game in runtime.
	float timeScale = 1.0f; // 1 --> Real Time / < 1 --> Slower / > 1 Faster, 0 Pause or Stop
	// Delta Time: last frame time expressed in seconds (Game Clock)
	Timer timer; // Used for gameDt (resets each frame)
	float gameDt = 0.0f;
	float gameClockSpeed = 0.0f; // != 0 (timeScale) --> Playing / == 0 --> Pause or Stop
	// Real Time Since Startup: seconds since game start (Real Time Clock) --> Engine.cpp
	// Real Time Delta Time: last frame time expressed in seconds (Real Time Clock) --> Engine.cpp
	// --------------------------------------------------

	float gameTime = 0.0f;
	
	GameState runtimeState = GameState::STOPPED;
};

#endif // !__SCENE_MANAGER_H__