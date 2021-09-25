#include "ImGUIHandler.h"
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

SceneIntro::SceneIntro(Camera3D* camera, Window* window, Renderer3D* renderer, ImGuiHandler* imGUIHandler) : Module()
{
	// Needed modules
	this->camera = camera;
	this->window = window;
	this->renderer = renderer;
	this->imGUIHandler = imGUIHandler;

	check = true;
	RNG rng;
	random = rng.GetRandomInt(0,35);
	std::ifstream stream("EngineConfig/window_test.json");

	SDL_assert(stream.is_open());
	try {
		j = nlohmann::json::parse(stream);
	}
	catch (nlohmann::json::parse_error& e) {
		LOG("Parse Error while Loading File: %c", e.what());
	}

	stream.close();


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


	
	imGUIHandler->CreateWin("Test JSON parser", j.at("Text").dump(4).c_str(),350.0f,450.0f);
	imGUIHandler->CreateWin("Test Random Number Generator", std::to_string(random).c_str());


	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}
