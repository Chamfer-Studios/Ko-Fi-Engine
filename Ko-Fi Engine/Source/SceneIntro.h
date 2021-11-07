#pragma once
#ifndef SCENE_INTRO_H
#define SCENE_INTRO_H
#include "Module.h"
#include "Globals.h"
#include "JsonHandler.h"
#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;


class SceneIntro : public Module
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
	GameObject* GetGameObject(int id);
	std::vector<GameObject*> gameObjectList;

private:

	int random = 0;
	JsonHandler jsonHandler;
	Json j;
	KoFiEngine* engine = nullptr;

};

#endif SCENE_INTRO_H