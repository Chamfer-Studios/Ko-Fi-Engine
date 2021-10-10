#include "Editor.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Camera3D.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Primitive.h"
#include <iostream>
#include <fstream>
#include "SDL_assert.h"
#include "RNG.h"
#include "ImGuiAppLog.h"

SceneIntro::SceneIntro(Camera3D* camera, Window* window, Renderer3D* renderer, Editor* editor) : Module()
{
	name = "SceneIntro";
	// Needed modules
	this->camera = camera;
	this->window = window;
	this->renderer = renderer;
	this->editor = editor;

	jsonHandler.LoadJson(j,"EngineConfig/window_test.json");
}

SceneIntro::~SceneIntro()
{}

// Load assets
bool SceneIntro::Start()
{
	LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");
	bool ret = true;

	window->SetTitle(jsonHandler.JsonToString(j.at("Text")).c_str());

	camera->Move(vec3(1.0f, 1.0f, 1.0f));
	camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Update
bool SceneIntro::Update(float dt)
{
	/*Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	return true;
}

// Load assets
bool SceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	appLog->AddLog("Unloading Intro scene\n");

	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}
