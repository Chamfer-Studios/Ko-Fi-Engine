#pragma once
#ifndef SCENE_INTRO_H
#define SCENE_INTRO_H

#include "Scene.h"
//#include "Globals.h"
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

	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:
	int random = 0;
	JsonHandler jsonHandler;
	Json j;

	// Temporal to manage the use of the camera
	GameObject* camera = nullptr;



};

#endif SCENE_INTRO_H // SCENE_INTRO_H