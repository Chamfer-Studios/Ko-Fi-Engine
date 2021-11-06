#pragma once
#ifndef SCENE_INTRO_H
#define SCENE_INTRO_H
#include "Module.h"
#include "Globals.h"
#include "JsonHandler.h"
#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class Camera3D;
class Renderer3D;
class Window;
class Editor;
class FileSystem;

class SceneIntro : public Module
{
public:
	SceneIntro(Camera3D* camera, Window* window, Renderer3D* renderer, Editor* editor, FileSystem* fileSystem);
	~SceneIntro();

	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:
	// Needed modules
	Camera3D* camera = nullptr;
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	Editor* editor = nullptr;
	FileSystem* fileSystem = nullptr;

	int random = 0;
	JsonHandler jsonHandler;
	Json j;
};

#endif SCENE_INTRO_H