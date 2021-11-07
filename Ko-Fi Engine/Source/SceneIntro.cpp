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
#include "FileSystem.h"
#include "GameObject.h"

#include "ComponentMaterial.h" // Temporal for the assignment, just to display the texture on the model when the program begins...

SceneIntro::SceneIntro(Camera3D* camera, Window* window, Renderer3D* renderer, Editor* editor, FileSystem* fileSystem) : Module()
{
	name = "SceneIntro";
	// Needed modules
	this->camera = camera;
	this->window = window;
	this->renderer = renderer;
	this->editor = editor;
	this->fileSystem = fileSystem;

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

	// Load initial scene (temporal)
	fileSystem->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_PLANE, editor->gameObjects);
	fileSystem->GameObjectFromMesh("Assets/Meshes/baker_house.fbx", editor->gameObjects);
	// Temporal for the assignment, just to display the texture on the model when the program begins...
	/*uint textureID = 1; ((ComponentMaterial*)editor->gameObjects.at(1)->GetComponent(COMPONENT_TYPE::COMPONENT_MESH))->LoadTexture((uint&) textureID, "Assets/Images/baker_house.png");
	textureID = 2; ((ComponentMaterial*)editor->gameObjects.at(1)->GetComponent(COMPONENT_TYPE::COMPONENT_MESH))->LoadTexture((uint&)textureID, "Assets/Images/baker_house.png");*/
	
	for (GameObject* go : editor->gameObjects)
	{
		go->Start();
	}
	return ret;
}

bool SceneIntro::PreUpdate(float dt)
{
	for (GameObject* go : editor->gameObjects)
	{
		go->PreUpdate();
	}
	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	for (GameObject* go : editor->gameObjects)
	{
		go->Update();
	}

	return true;
}

bool SceneIntro::PostUpdate(float dt)
{
	// Draw meshes
	for (GameObject* go : editor->gameObjects)
	{
		go->PostUpdate();
	}

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
