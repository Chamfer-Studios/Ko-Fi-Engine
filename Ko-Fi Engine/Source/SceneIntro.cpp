#include "Editor.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Engine.h"
#include "Camera3D.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Primitive.h"
#include "ImGuiAppLog.h"
#include "FileSystem.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentScript.h"
#include "Material.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "node_editor.h"

#include "SDL_assert.h"

SceneIntro::SceneIntro(KoFiEngine* engine) : Scene()
{
	name = "SceneIntro";

	// Needed modules
	this->engine = engine;

	jsonHandler.LoadJson(j,"EngineConfig/window_test.json");

	rootGo = new GameObject(-1, engine, "Root");
	rootGo->SetParentUID(rootGo->GetUID());
	gameObjectList.push_back(rootGo);
}

SceneIntro::~SceneIntro()
{
	CleanUp();
}

// Load assets
bool SceneIntro::Start()
{
	bool ret = true;

	CONSOLE_LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");

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
	//engine->GetSceneManager()->LoadScene(this, "SceneIntro");

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
	for (GameObject* go : this->gameObjectList)
	{
		go->Update(dt);
	}
	//example::NodeEditorShow();

	return true;
}

bool SceneIntro::PostUpdate(float dt)
{
	// Draw meshes
	for (GameObject* go : gameObjectList)
	{
		go->PostUpdate(dt); 
	}
	for (GameObject* parent : gameObjectListToCreate)
	{
		GameObject* bullet = CreateEmptyGameObject("Bullet");
		parent->GetComponent<ComponentScript>()->handler->lua["bullet"] = bullet;
		parent->GetComponent<ComponentScript>()->handler->lua.script("table.insert(bullets, bullet)");
		parent->GetComponent<ComponentScript>()->handler->lua.script("print(bullets[1]:GetTransform():GetPosition().x)");

		bullet->GetTransform()->SetScale(float3(0.025, 0.05, 0.08));
		float3 pos = parent->GetTransform()->GetPosition();
		bullet->GetTransform()->SetPosition(float3(pos.x, pos.y + 1, pos.z));
		bullet->GetTransform()->SetRotation(parent->GetTransform()->GetRotation());

		ComponentMesh* componentMesh = bullet->CreateComponent<ComponentMesh>();
		Mesh* mesh = gameObjectList.at(2)->GetComponent<ComponentMesh>()->GetMesh();
		componentMesh->SetMesh(mesh);

		
		ComponentMaterial* componentMaterial = bullet->CreateComponent<ComponentMaterial>();
		Importer::GetInstance()->textureImporter->Import(nullptr,&componentMaterial->texture);
		Material* material = new Material();
		Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material);
		componentMaterial->SetMaterial(material);
		

		ComponentScript* componentScript = bullet->CreateComponent<ComponentScript>();
		componentScript->script = componentScript->handler->lua.load_file("Assets/Scripts/Bullet.lua");
		componentScript->script();
		componentScript->SetRunning(true);
		parent->GetComponent<ComponentScript>()->handler->lua["SetBulletDirection"](bullet);
	}
	gameObjectListToCreate.clear();
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