#pragma once
#include "Module.h"
#include "Globals.h"
#include "json.hpp"
#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class Camera3D;
class Renderer3D;
class Window;
class ImGuiHandler;

class SceneIntro : public Module
{
public:
	SceneIntro(Camera3D* camera, Window* window, Renderer3D* renderer, ImGuiHandler* imGuiHandler);
	~SceneIntro();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:
	// Needed modules
	Camera3D* camera;
	Window* window;
	Renderer3D* renderer;
	ImGuiHandler* imGUIHandler;

	bool check = true;
	int random = 0;
	nlohmann::json j;
};