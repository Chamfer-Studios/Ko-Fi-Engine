#pragma once
#include "Module.h"
#include "Globals.h"


#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class Camera3D;
class Renderer3D;
class Window;

class SceneIntro : public Module
{
public:
	SceneIntro(Camera3D* camera,Window* window,Renderer3D* renderer);
	~SceneIntro();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:
	Camera3D* camera;
	Window* window;
	Renderer3D* renderer;
	bool check = true;
	int random = 0;
};