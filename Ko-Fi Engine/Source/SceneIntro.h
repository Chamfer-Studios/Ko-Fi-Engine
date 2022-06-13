#ifndef __SCENE_INTRO_H__
#define __SCENE_INTRO_H__

#include "Scene.h"
#include "JsonHandler.h"

#define MAX_SNAKE 2

class GameObject;
struct PhysBody3D;
struct PhysMotor3D;

class SceneIntro : public Scene
{
public:
	SceneIntro(KoFiEngine* engine);
	~SceneIntro();

	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	bool switchScene = false;
	bool quitPlease = false;
	std::string sceneNameGO;

private:
	int random = 0;
	JsonHandler jsonHandler;
	Json j;
	// Temporal to manage the use of the camera
	GameObject* camera = nullptr;
};

#endif // !__SCENE_INTRO_H__
