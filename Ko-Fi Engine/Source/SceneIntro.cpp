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
#include "ComponentCamera.h"
#include "ComponentTransform2D.h"
#include "GameObject.h"
//#include "Importer.h"
#include "SceneManager.h"
#include "node_editor.h"

#include "ComponentMaterial.h" // Temporal for the assignment, just to display the texture on the model when the program begins...

SceneIntro::SceneIntro(KoFiEngine* engine) : Scene()
{
	name = "SceneIntro";
	// Needed modules
	this->engine = engine;

	jsonHandler.LoadJson(j,"EngineConfig/window_test.json");
	rootGo = new GameObject(-1, engine, "Root");
	gameObjectList.push_back(rootGo);
}

SceneIntro::~SceneIntro()
{
	CleanUp();
}

// Load assets
bool SceneIntro::Start()
{
	CONSOLE_LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");
	bool ret = true;
	example::NodeEditorInitialize();

	// Load initial scene (temporal)
	//engine->GetFileSystem()->GameObjectFromMesh("Assets/Models/baker_house.fbx", this->gameObjectList,"Assets/Textures/baker_house.png");

	// REMOVE THE FOLLOWING 2 LINES WHEN WE HAVE THE CUSTOM FILE FORMAT FINISHED.
	//Importer::GetInstance()->ImportModel("Assets/Models/baker_house.fbx");
	/*Importer::GetInstance()->ImportModel("Assets/Models/camera.fbx");
	GameObject* camera = engine->GetSceneManager()->GetCurrentScene()->GetGameObject(2);
	ComponentCamera* componentCamera = camera->CreateComponent<ComponentCamera>();
	camera->AddComponent(componentCamera);*/

	// Load scene with a camera and several houses.
	engine->GetSceneManager()->LoadScene(this, "SceneIntro");

	return ret;
}

bool SceneIntro::PreUpdate(float dt)
{
	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() != nullptr)
			go->PreUpdate();
	}

	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() != nullptr)
			go->Update();
	}
	//example::NodeEditorShow();

	return true;
}

bool SceneIntro::PostUpdate(float dt)
{
	// Draw meshes
	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() != nullptr)
			go->PostUpdate(dt);
	}

	engine->GetRenderer()->DrawRay();
	return true;
}

// Load assets
bool SceneIntro::CleanUp()
{
	CONSOLE_LOG("Unloading Intro scene");
	appLog->AddLog("Unloading Intro scene\n");

	for (GameObject* gameObject : gameObjectList)
	{
		RELEASE(gameObject);
	}

	example::NodeEditorShutdown();

	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}
