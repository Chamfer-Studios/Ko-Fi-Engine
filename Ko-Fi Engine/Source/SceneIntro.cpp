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
#include "ComponentParticle.h"
#include "ComponentScript.h"
#include "Scripting.h" // Consider moving this to Globals.h or smth
#include "ComponentTransform.h"
#include "Material.h"
#include "ComponentTransform2D.h"
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

	rootGo = new GameObject(-1, engine, "SceneIntro");
	rootGo->SetParentUID(rootGo->GetUID());
	gameObjectList.push_back(rootGo);

	//LCG random;
	//uint uid = random.Int();
	//GameObject * g = this->CreateEmptyGameObject("Particle Test");
	//g->AddComponentByType(ComponentType::PARTICLE);//CreateComponent<ComponentParticle>();

	
}

SceneIntro::~SceneIntro()
{
	CleanUp();
}

// Load assets
bool SceneIntro::Start()
{
	bool ret = true;
	GameObject* camera = CreateEmptyGameObject("camera");
	ComponentCamera* cCamera = camera->CreateComponent<ComponentCamera>();
	cCamera->isMainCamera = true;
	engine->GetCamera3D()->SetGameCamera(cCamera);
	CONSOLE_LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");

	example::NodeEditorInitialize();

	for (GameObject* go : this->gameObjectList)
	{
		go->Start();
	}

	ComputeQuadTree();
	
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
	if (ray.IsFinite())
		//DrawDebugRay(ray);

	if (sceneTree != nullptr && drawSceneTree)
	{
		ComputeQuadTree();
		sceneTree->Draw();
	}
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
		//parent->GetComponent<ComponentScript>()->handler->lua["bullet"] = bullet;
		//parent->GetComponent<ComponentScript>()->handler->lua.script("table.insert(bullets, bullet)"); //We will need something like this

		bullet->GetTransform()->SetScale(float3(0.1, 0.1, 0.1));
		float3 pos = parent->GetTransform()->GetPosition();
		bullet->GetTransform()->SetPosition(float3(pos.x, pos.y + 15, pos.z - 15));
		float3 parentRot = parent->GetTransform()->GetRotationEuler();
		float3 rot = { parentRot.x - 55,parentRot.y,parentRot.z };
		bullet->GetTransform()->SetRotationEuler(rot);

		ComponentMesh* componentMesh = bullet->CreateComponent<ComponentMesh>();
 		Mesh* mesh = gameObjectList.at(7)->GetComponent<ComponentMesh>()->GetMesh();
		componentMesh->SetMesh(mesh);
		
		ComponentMaterial* componentMaterial = bullet->CreateComponent<ComponentMaterial>();
		Importer::GetInstance()->textureImporter->Import(nullptr, &componentMaterial->texture);
		Material* material = new Material();
		Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material);
		componentMaterial->SetMaterial(material);
		
		ComponentScript* componentScript = (ComponentScript*)bullet->AddComponentByType(ComponentType::SCRIPT);//CreateComponent<ComponentScript>();
		componentScript->path = "Assets/Scripts/Bullet.lua";
		componentScript->ReloadScript();
		GameObject* target = parent->GetComponent<ComponentScript>()->handler->lua["target"];
		componentScript->handler->lua["target"] = target;
		componentScript->handler->lua["SetDestination"]();
	}
	gameObjectListToCreate.clear();
	for (GameObject* gameObject : gameObjectListToDelete)
	{
		DeleteGameObject(gameObject);
	}
	gameObjectListToDelete.clear();

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

	lights.clear();

	example::NodeEditorShutdown();

	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}