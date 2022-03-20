#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "glmath.h"
#include "ComponentCamera.h"

class GameObject;

class Camera3D : public Module
{
public:
	Camera3D(KoFiEngine* engine);
	~Camera3D();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	// Method to receive and manage events
	void OnNotify(const Event& event);

	void OnGui() override;
	void OnClick(SDL_Event event);

	void OnPlay();
	void OnStop();
	//void OnSave(JSONWriter& writer) const override;
	//void OnLoad(const JSONReader& reader) override;

	void CheckInput(float dt);
	void CheckMouseMotion();

	void SetGameCamera(ComponentCamera* gameCamera);

	GameObject* MousePicking();

public:
	GameObject* engineCameraObject = nullptr; // The engine camera needs a game object as holder if we want to be able to access "engine" from component camera.

	ComponentCamera* currentCamera = nullptr; // The camera that will display on screen
	ComponentCamera* engineCamera = nullptr; // The engine camera, asigning this to currentCamera will display de engine camera
	ComponentCamera* gameCamera = nullptr; // The game camera, asigning this to currentCamera will display de game camera

private:

	KoFiEngine* engine = nullptr;
};

#endif // !__CAMERA_H__