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
#include "C_Button.h"
#include "C_Script.h"
#include "C_Transform.h"
#include "C_LightSource.h"
#include "C_Transform2D.h"
#include "C_Particle.h"

#include "Scripting.h" // Consider moving this to Globals.h or smth
#include "Primitive.h"
#include "ImGuiAppLog.h"
#include "R_Material.h"
#include "Log.h"
#include "node_editor.h"
#include "Quadtree.h"

#include "SDL_assert.h"

SceneIntro::SceneIntro(KoFiEngine *engine) : Scene()
{
	name = "SceneIntro";

	// Needed modules
	this->engine = engine;

	jsonHandler.LoadJson(j, "EngineConfig/window_test.json");

	rootGo = new GameObject(0, engine, "Root");
	rootGo->SetParentUID(rootGo->GetUID());
	gameObjectList.push_back(rootGo);

	skybox = SkyBox();
	// LCG random;
	// uint uid = random.Int();
	// GameObject * g = this->CreateEmptyGameObject("Particle Test");
	// g->AddComponentByType(ComponentType::PARTICLE);
}

SceneIntro::~SceneIntro()
{
	CleanUp();
}

// Load assets
bool SceneIntro::Start()
{
	bool ret = true;
	skybox.Start();
#ifdef KOFI_GAME
	bool mouseChanged = engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/MouseDefault.bmp");
	LOG("Mouse has changed: %d", mouseChanged);
#endif // KOFI_GAME


	// Load Default Screen (Can be changed from settings)
	if (!engine->GetSceneManager()->GetDefaultScene().empty())
	{
		Importer::GetInstance()->sceneImporter->LoadScene(this, engine->GetSceneManager()->GetDefaultScene().c_str());
	}
	else
	{
		// TODO: Load a default scene, or create a new empty one
	}
	if (!engine->GetCamera3D()->gameCamera)
	{
		GameObject *camera = CreateEmptyGameObject("Main Camera");
		C_Camera *cCamera = (C_Camera*)camera->AddComponentByType(ComponentType::CAMERA);
		cCamera->SetIsMainCamera(true);
		engine->GetCamera3D()->SetGameCamera(cCamera);


	}
#ifdef KOFI_GAME
	engine->GetCamera3D()->currentCamera = engine->GetCamera3D()->gameCamera;
	engine->GetSceneManager()->OnPlay();
#endif //KOFI_ENGINE

	if (this->GetLights(SourceType::DIRECTIONAL).size() == 0)
	{
		//create dir light?
	}
	CONSOLE_LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");

	example::NodeEditorInitialize();

	//ComputeQuadTree();
	sceneTree.SetBoundaries(AABB(float3(-500, 0, -500), float3(500, 30, 500)));


	return ret;
}

bool SceneIntro::PreUpdate(float dt)
{
	for (std::map<std::string, std::string>::iterator mapIt = gameObjectListToCreate.begin(); mapIt != gameObjectListToCreate.end(); mapIt++)
	{
		std::string path = "Assets/Prefabs/" + (*mapIt).second + "_prefab.json";
		if (!std::filesystem::exists(path))
			continue;
		GameObject* go = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
		go->LoadPrefabJson(path.c_str(), false);
		if (std::string(go->GetName()) != (*mapIt).first)
			go->SetName(((*mapIt).first).c_str(), (*mapIt).first == (*mapIt).second);
		for (Component* component : go->GetComponents()) // This method used because there could be multiple scripts in one go
		{
			if (component->GetType() != ComponentType::SCRIPT)
				continue;

			C_Script* script = (C_Script*)component;

			auto start = sol::protected_function(script->s->handler->lua["Start"]);
			if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING && script->s->isScriptLoaded)
			{
				if (start.valid()) {
					sol::protected_function_result result = start();
					if (result.valid()) {
						// Call succeeded
					}
					else {
						// Call failed
						sol::error err = result;
						std::string what = err.what();
						appLog->AddLog("%s\n", what.c_str());
					}
				}
			}
		}
	}
	gameObjectListToCreate.clear();

	for (GameObject *go : this->gameObjectList)
	{
		go->PreUpdate();
	}

	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	OPTICK_EVENT();

	for (int i = 0; i < this->gameObjectList.size(); i++)
	{
		GameObject* go = this->gameObjectList[i];
		go->Update(dt);
		if (go->changeScene)
		{
			switchScene = true;
			sceneNameGO = go->sceneName;
			go->changeScene = false;
		}
		if (go->isQuitting)
		{
			quitPlease = true;
			go->isQuitting = false;
		}
	}

	for (int i = 0; i < this->gameObjectList.size(); i++)
	{
		GameObject* go = this->gameObjectList[i];
		go->InitUpdateScripts(dt);
	}

	for (int i = 0; i < this->gameObjectList.size(); i++)
	{
		GameObject* go = this->gameObjectList[i];
		go->DoUpdateScripts(dt);
	}

#pragma omp parallel for
	for (int i = 0; i < this->gameObjectList.size(); i++)
	{
		GameObject* go = this->gameObjectList[i];
		go->DoUpdateAsyncScripts(dt);
	}

	// example::NodeEditorShow();
	//if (ray.IsFinite())
		// DrawDebugRay(ray);

	
	return true;
}

bool SceneIntro::PostUpdate(float dt)
{
	// Draw meshes
	for (GameObject *go : gameObjectList)
	{
		go->PostUpdate(dt);
	}

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
		Importer::GetInstance()->sceneImporter->LoadScene(this, sceneNameGO.c_str());
		engine->GetSceneManager()->OnSceneSwitch();
	}

	if (quitPlease)
	{
		quitPlease = false;
		engine->GetSceneManager()->OnStop();
		engine->GetCamera3D()->OnStop();
	}

	if (engine->GetInput()->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		for (int i = 0; i < engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size(); i++)
		{
			DeleteGameObject(GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[i]));
		}
	}

#ifdef KOFI_GAME
	SwitchCursor([this](std::string path) {engine->GetSceneManager()->ChangeMouseTexture(path); }, [this]() {engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/MouseDefault.bmp"); });
	//OnAnyButtonHovered([this]() {engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/MouseUI.bmp"); }, [this]() {engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/MouseDefault.bmp"); });
	//OnAnyEnemyHovered([this]() {engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/mouseAttack.bmp"); }, [this]() {engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/MouseDefault.bmp"); });
	//OnAnySpiceSpotHovered([this]() {engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/mousePick.bmp"); }, [this]() {engine->GetSceneManager()->ChangeMouseTexture("Assets/New UI/MouseDefault.bmp"); });
#endif // KOFI_GAME


	return true;
}




// Load assets
bool SceneIntro::CleanUp()
{
	CONSOLE_LOG("Unloading Intro scene");
	appLog->AddLog("Unloading Intro scene\n");

	Scene::CleanUp();

	example::NodeEditorShutdown();

	
	camera = nullptr;

	return true;
}

void SceneIntro::OnCollision(PhysBody3D *body1, PhysBody3D *body2)
{
}