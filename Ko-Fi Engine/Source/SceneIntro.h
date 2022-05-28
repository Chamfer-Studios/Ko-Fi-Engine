#ifndef __SCENE_INTRO_H__
#define __SCENE_INTRO_H__

#include "Scene.h"
#include "JsonHandler.h"

#define MAX_SNAKE 2

class GameObject;
struct PhysBody3D;
struct PhysMotor3D;
struct M_SceneManager;

class SceneIntro : public Scene
{
public:
	SceneIntro(KoFiEngine* engine);
	~SceneIntro();

	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void OnAnyButtonHovered(const std::function<void()>& onAnyButtonHovered, const std::function<void()>& onNoButtonHovered);

	bool switchScene = false;
	bool quitPlease = false;
	std::string sceneNameGO;

private:
	int random = 0;
	JsonHandler jsonHandler;
	Json j;
	// Temporal to manage the use of the camera
	GameObject* camera = nullptr;
	M_SceneManager* sceneManager = nullptr;
};

#endif // !__SCENE_INTRO_H__
