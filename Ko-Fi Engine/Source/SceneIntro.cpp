#include "Globals.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Camera3D.h"
#include "Primitive.h"
#include "imgui.h"

SceneIntro::SceneIntro(Camera3D* camera) : Module()
{
	this->camera = camera;
}

SceneIntro::~SceneIntro()
{}

// Load assets
bool SceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	camera->Move(vec3(1.0f, 1.0f, 0.0f));
	camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool SceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}

