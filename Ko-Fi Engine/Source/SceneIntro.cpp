#include "SceneIntro.h"

// Modules
#include "Engine.h"
#include "M_Editor.h"
#include "M_Camera3D.h"
#include "M_Renderer3D.h"
#include "M_Window.h"
#include "M_FileSystem.h"
#include "M_SceneManager.h"

// GameObject
#include "GameObject.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"
#include "C_Script.h"
#include "C_Collider.h"
#include "C_Transform.h"
#include "C_LightSource.h"
#include "C_Transform2D.h"
#include "ComponentParticle.h"

#include "Scripting.h" // Consider moving this to Globals.h or smth
#include "Primitive.h"
#include "ImGuiAppLog.h"
#include "R_Material.h"
#include "Log.h"
#include "node_editor.h"
#include "QuadTree3D.h"

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
		GameObject *camera = CreateEmptyGameObject("Main Camera");
		C_Camera *cCamera = camera->CreateComponent<C_Camera>();
		cCamera->isMainCamera = true;
		engine->GetCamera3D()->SetGameCamera(cCamera);
	}
	if (this->GetLights(SourceType::DIRECTIONAL).size() == 0)
	{
		GameObject* go = CreateEmptyGameObject("Direct Light");
		go->AddComponentByType(ComponentType::LIGHT_SOURCE);
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
	for (GameObject* go : this->gameObjectList)
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
	//if (ray.IsFinite())
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
		GameObject* parent = (*mapIt).first;
		if ((*mapIt).second == "Knife" || (*mapIt).second == "Dart")
		{
			GameObject* karambit = parent->GetComponent<C_Script>()->s->handler->LuaFind("Karambit");
			if (!karambit)
				continue;

			GameObject* goIt = CreateEmptyGameObject((*mapIt).second.c_str());

			goIt->tag = Tag::TAG_PROJECTILE;
			C_RigidBody *rigidBody = goIt->CreateComponent<C_RigidBody>();

			goIt->GetTransform()->SetScale(float3(0.1, 0.1, 0.1));
			float3 pos = parent->GetTransform()->GetPosition();

			rigidBody->SetRigidBodyPos(float3(pos.x, pos.y, pos.z));
			goIt->GetTransform()->SetPosition(float3(pos.x, pos.y, pos.z - 15));

			C_Mesh *componentMesh = goIt->CreateComponent<C_Mesh>();

			R_Mesh* mesh = karambit->GetComponent<C_Mesh>()->GetMesh();
			componentMesh->SetMesh(mesh);

			C_Material *cMaterial = goIt->CreateComponent<C_Material>();
			R_Material *material = parent->GetComponent<C_Script>()->s->handler->LuaFind("Karambit")->GetComponent<C_Material>()->GetMaterial();

			cMaterial->SetMaterial(material);

			rigidBody->FreezePositionY(true);
			C_Collider *collider = goIt->CreateComponent<C_Collider>();
			collider->SetColliderShape(ColliderShape::BOX);
			collider->SetFilter("projectile");
			collider->SetIsTrigger(true);

			C_Script *knifeScript = (C_Script *)goIt->AddComponentByType(ComponentType::SCRIPT); // CreateComponent<C_Script>();
			knifeScript->s->path = "Assets/Scripts/Players/Zhib/Knife.lua";
			knifeScript->ReloadScript(knifeScript->s);
			GameObject *target = parent->GetComponent<C_Script>()->s->handler->lua["target"];
			knifeScript->s->handler->lua["target"] = target;
			knifeScript->s->handler->lua["SetDestination"]();
		}
		else if ((*mapIt).second == "Decoy")
		{
			GameObject* decoy = parent->GetComponent<C_Script>()->s->handler->LuaFind("Decoy");
			if (!decoy)
				continue;

			GameObject* goIt = CreateEmptyGameObject((*mapIt).second.c_str());

			goIt->tag = Tag::TAG_PROJECTILE;
			C_RigidBody* rigidBody = goIt->CreateComponent<C_RigidBody>();

			float3 pos = parent->GetTransform()->GetPosition();
			rigidBody->SetRigidBodyPos(float3(pos.x, pos.y, pos.z));
			goIt->GetTransform()->SetPosition(float3(pos.x, pos.y, pos.z - 15));
			goIt->GetTransform()->SetScale(float3(0.01, 0.01, 0.005));
			C_Mesh* componentMesh = goIt->CreateComponent<C_Mesh>();
			R_Mesh* mesh = decoy->GetComponent<C_Mesh>()->GetMesh();
			componentMesh->SetMesh(mesh);

			C_Material* cMaterial = goIt->CreateComponent<C_Material>();
			R_Material* material = parent->GetComponent<C_Script>()->s->handler->LuaFind("Karambit")->GetComponent<C_Material>()->GetMaterial();
			cMaterial->SetMaterial(material);

			rigidBody->FreezePositionY(true);
			C_Collider* collider = goIt->CreateComponent<C_Collider>();
			collider->SetColliderShape(ColliderShape::BOX);
			collider->SetFilter("terrain");

			C_Script* decoyScript = (C_Script*)goIt->AddComponentByType(ComponentType::SCRIPT); // CreateComponent<C_Script>();
			decoyScript->s->path = "Assets/Scripts/Players/Zhib/Decoy.lua";
			decoyScript->ReloadScript(decoyScript->s);
			GameObject* target = parent->GetComponent<C_Script>()->s->handler->lua["target"];
			decoyScript->s->handler->lua["target"] = target;
			decoyScript->s->handler->lua["SetDestination"]();
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

	if (engine->GetInput()->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		DeleteGameObject(GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID));
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