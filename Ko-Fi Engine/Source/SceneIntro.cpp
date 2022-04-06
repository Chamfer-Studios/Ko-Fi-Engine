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
#include "C_Collider.h"
#include "Scripting.h" // Consider moving this to Globals.h or smth
#include "ComponentTransform.h"
#include "Material.h"
#include "ComponentTransform2D.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "node_editor.h"

#include "SDL_assert.h"

SceneIntro::SceneIntro(KoFiEngine *engine) : Scene()
{
	name = "SceneIntro";

	// Needed modules
	this->engine = engine;

	jsonHandler.LoadJson(j, "EngineConfig/window_test.json");

	rootGo = new GameObject(-1, engine, "Root");
	rootGo->SetParentUID(rootGo->GetUID());
	gameObjectList.push_back(rootGo);

	// LCG random;
	// uint uid = random.Int();
	// GameObject * g = this->CreateEmptyGameObject("Particle Test");
	// g->AddComponentByType(ComponentType::PARTICLE);//CreateComponent<ComponentParticle>();
}

SceneIntro::~SceneIntro()
{
	CleanUp();
}

// Load assets
bool SceneIntro::Start()
{
	bool ret = true;
	// Load Default Screen (Can be changed from settings)
	if (!engine->GetSceneManager()->GetDefaultScene().empty())
	{
		Importer::GetInstance()->sceneImporter->Load(this, engine->GetSceneManager()->GetDefaultScene().c_str());
	}
	if (!engine->GetCamera3D()->gameCamera)
	{
		GameObject *camera = CreateEmptyGameObject("camera");
		ComponentCamera *cCamera = camera->CreateComponent<ComponentCamera>();
		cCamera->isMainCamera = true;
		engine->GetCamera3D()->SetGameCamera(cCamera);
	}

	CONSOLE_LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");

	example::NodeEditorInitialize();

	for (GameObject *go : this->gameObjectList)
	{
		go->Start();
	}

	ComputeQuadTree();

	return ret;
}

bool SceneIntro::PreUpdate(float dt)
{
	for (GameObject *go : this->gameObjectList)
	{
		go->PreUpdate();
	}

	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	for (GameObject *go : this->gameObjectList)
	{
		go->Update(dt);
		if (go->changeScene)
		{
			switchScene = true;
			sceneNameGO = go->sceneName;
			go->changeScene = false;
		}
	}

	// example::NodeEditorShow();
	if (ray.IsFinite())
		// DrawDebugRay(ray);

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
	for (GameObject *go : gameObjectList)
	{
		go->PostUpdate(dt);
	}

	for (std::map<GameObject *, std::string>::iterator mapIt = gameObjectListToCreate.begin(); mapIt != gameObjectListToCreate.end(); mapIt++)
	{
		GameObject *knife = CreateEmptyGameObject((*mapIt).second.c_str());
		GameObject *parent = (*mapIt).first;

		if ((*mapIt).second == "Knife" || (*mapIt).second == "Dart")
		{
			knife->tag = Tag::TAG_PROJECTILE;
			ComponentRigidBody *rigidBody = knife->CreateComponent<ComponentRigidBody>();

			knife->GetTransform()->SetScale(float3(0.1, 0.1, 0.1));
			float3 pos = parent->GetTransform()->GetPosition();

			rigidBody->SetRigidBodyPos(float3(pos.x, pos.y + 15, pos.z - 15));
			float3 parentRot = parent->GetTransform()->GetRotationEuler();
			knife->GetTransform()->SetPosition(float3(pos.x, pos.y + 15, pos.z - 15));
			float3 rot = {parentRot.x - 55, parentRot.y, parentRot.z};
			knife->GetTransform()->SetRotationEuler(rot);

			ComponentMesh *componentMesh = knife->CreateComponent<ComponentMesh>();
			Mesh *mesh = parent->GetComponent<ComponentScript>()->handler->LuaFind("Karambit")->GetComponent<ComponentMesh>()->GetMesh();
			componentMesh->SetMesh(mesh);

			ComponentMaterial *componentMaterial = knife->CreateComponent<ComponentMaterial>();
			//Importer::GetInstance()->textureImporter->Import(nullptr, &componentMaterial->texture);
			Material *material = parent->GetComponent<ComponentScript>()->handler->LuaFind("Karambit")->GetComponent<ComponentMaterial>()->GetMaterial();
			//Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material);
			componentMaterial->SetMaterial(material);

			rigidBody->FreezePositionY(true);
			ComponentCollider2 *collider = knife->CreateComponent<ComponentCollider2>();
			collider->SetColliderShape(ColliderShape::BOX);
			collider->SetFilter("projectile");
			collider->SetIsTrigger(true);

			ComponentScript *knifeScript = (ComponentScript *)knife->AddComponentByType(ComponentType::SCRIPT); // CreateComponent<ComponentScript>();
			knifeScript->path = "Assets/Scripts/Knife.lua";
			knifeScript->ReloadScript();
			GameObject *target = parent->GetComponent<ComponentScript>()->handler->lua["target"];
			knifeScript->handler->lua["target"] = target;
			knifeScript->handler->lua["SetDestination"]();
		}
	}
	gameObjectListToCreate.clear();
	for (GameObject* gameObject : gameObjectListToDelete)
	{
		DeleteGameObject(gameObject);
	}
	gameObjectListToDelete.clear();
	gameObjectListToDelete.shrink_to_fit();

	engine->GetRenderer()->DrawRay();

	if (switchScene)
	{
		switchScene = false;
		Importer::GetInstance()->sceneImporter->Load(this, sceneNameGO.c_str());
	}

	return true;
}

// Load assets
bool SceneIntro::CleanUp()
{
	CONSOLE_LOG("Unloading Intro scene");
	appLog->AddLog("Unloading Intro scene\n");

	for (GameObject *gameObject : gameObjectList)
	{
		RELEASE(gameObject);
	}
	gameObjectList.clear();
	gameObjectList.shrink_to_fit();

	lights.clear();

	example::NodeEditorShutdown();

	return true;
}

void SceneIntro::OnCollision(PhysBody3D *body1, PhysBody3D *body2)
{
}