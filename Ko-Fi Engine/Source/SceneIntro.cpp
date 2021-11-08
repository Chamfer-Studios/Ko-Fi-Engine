#include "Editor.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Engine.h"
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
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "GameObject.h"

#include "ComponentMaterial.h" // Temporal for the assignment, just to display the texture on the model when the program begins...

SceneIntro::SceneIntro(KoFiEngine* engine) : Module()
{
	name = "SceneIntro";
	// Needed modules
	this->engine = engine;

	jsonHandler.LoadJson(j,"EngineConfig/window_test.json");
	rootGo = new GameObject(-1,"Root");
	gameObjectList.push_back(rootGo);
}

SceneIntro::~SceneIntro()
{}

// Load assets
bool SceneIntro::Start()
{
	LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");
	bool ret = true;

	engine->GetWindow()->SetTitle(jsonHandler.JsonToString(j.at("Text")).c_str());

	engine->GetCamera3D()->Move(vec3(1.0f, 1.0f, 1.0f));
	engine->GetCamera3D()->LookAt(vec3(0, 0, 0));

	// Load initial scene (temporal)
	engine->GetFileSystem()->GameObjectFromMesh("Assets/Meshes/baker_house.fbx", this->gameObjectList,"Assets/Images/baker_house.png");
	engine->GetFileSystem()->GameObjectFromMesh("Assets/Meshes/prop.fbx", this->gameObjectList,"Assets/Images/prop.jpg");
	engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE, this->gameObjectList);
	gameObjectList.at(2)->GetTransform()->SetPosition(5, 0, 0);
	gameObjectList.at(3)->GetTransform()->SetPosition(-5, 0, 0);


	for (GameObject* go : this->gameObjectList)
	{
		go->Start();
	}

	return ret;
}

bool SceneIntro::PreUpdate(float dt)
{
	for (GameObject* go : this->gameObjectList)
	{
		go->PreUpdate();
	}

	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	Plane plane(0, 0, 1, 0);
	plane.axis = true;
	plane.InnerRender();

	for (GameObject* go : this->gameObjectList)
	{
		go->Update();
	}

	return true;
}

bool SceneIntro::PostUpdate(float dt)
{
	// Draw meshes
	for (GameObject* go : this->gameObjectList)
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

	for (GameObject* gameObject : gameObjectList)
	{
		RELEASE(gameObject);
	}
	RELEASE(rootGo);

	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}

GameObject* SceneIntro::GetGameObject(int id)
{
	for (GameObject* go : gameObjectList)
	{
		if (go->GetId() == id)
		{
			return go;
		}
	}

	return nullptr;
}
