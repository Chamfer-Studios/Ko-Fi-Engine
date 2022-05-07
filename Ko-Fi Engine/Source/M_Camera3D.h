#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float2.h"


class GameObject;
class C_Camera;

class M_Camera3D : public Module
{
public:
	M_Camera3D(KoFiEngine* engine);
	~M_Camera3D();

	bool Awake(Json configModule);
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	// Method to receive and manage events
	void OnNotify(const Event& event);

	void OnGui() override;
	void OnClick(SDL_Event event);

	void OnPlay();
	void OnStop();

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------

	// Camera Functions
	// Inputs
	void CheckInput(float dt);
	void MouseZoom(float dt);
	void MouseRotation(float dt);
	void FocusTarget();

	// Setters
	void SetGameCamera(C_Camera* gameCamera);
	inline void ChangeSpeed(int speedMultiplayer) { 
		this->speedMultiplier = speedMultiplayer;
		cameraSpeed = baseCameraSpeed * speedMultiplayer; }
	inline int GetSpeedMultiplier() const { return speedMultiplier; }

	// Getters
	float3 GetLastMouseClick() const;
	
	GameObject* MousePicking(const bool& isRightButton = false);

	float2 WorldToScreen(float3 position);


public:
	GameObject* engineCameraObject = nullptr; // The engine camera needs a game object as holder if we want to be able to access "engine" from component camera.

	C_Camera* currentCamera = nullptr; // The camera that will display on screen
	C_Camera* engineCamera = nullptr; // The engine camera, asigning this to currentCamera will display de engine camera
	C_Camera* gameCamera = nullptr; // The game camera, asigning this to currentCamera will display de game camera

private:
	KoFiEngine* engine = nullptr;
	float3 lastMouseClick;

	float cameraSensitivity = .1f;
	float cameraSpeed = 1.f;
	float baseCameraSpeed = 10.f;
	float maxSpeed = 25.f;
	int speedMultiplier = 1.0f;

	float lastDeltaX = 0.f, lastDeltaY = 0.f;

	bool isMoving = false;


};

#endif // !__CAMERA_H__