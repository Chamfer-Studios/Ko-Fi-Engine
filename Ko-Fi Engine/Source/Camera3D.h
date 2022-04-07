#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "glmath.h"


class GameObject;
class C_Camera;

class Camera3D : public Module
{
public:
	Camera3D(KoFiEngine* engine);
	~Camera3D();

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

	void CheckInput(float dt);
	void CheckMouseMotion(float dt);

	void SetGameCamera(C_Camera* gameCamera);
	
	GameObject* MousePicking(const bool& isRightButton = false);

	float3 GetLastMouseClick() const;

public:
	GameObject* engineCameraObject = nullptr; // The engine camera needs a game object as holder if we want to be able to access "engine" from component camera.

	C_Camera* currentCamera = nullptr; // The camera that will display on screen
	C_Camera* engineCamera = nullptr; // The engine camera, asigning this to currentCamera will display de engine camera
	C_Camera* gameCamera = nullptr; // The game camera, asigning this to currentCamera will display de game camera

private:
	float3 lastMouseClick;
	KoFiEngine* engine = nullptr;
};

#endif // !__CAMERA_H__