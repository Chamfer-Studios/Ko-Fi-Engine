#include "M_SceneManager.h"
#include "Engine.h"
#include "M_Editor.h"
#include "SceneIntro.h"
#include "M_Camera3D.h"
#include "M_Window.h"
#include <imgui_stdlib.h>

#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Info.h"
#include "C_Camera.h"
#include "C_Script.h"
// UI
#include "C_Canvas.h"
#include "C_Transform2D.h"
#include "C_Image.h"
#include "C_Button.h"
#include "C_Text.h"

#include "R_Material.h"
#include "R_Texture.h"

#include "PanelViewport.h"
#include "Log.h"
#include "Scripting.h"

#include "Log.h"
#include "Globals.h"

#include "optick.h"

M_SceneManager::M_SceneManager(KoFiEngine* engine)
{
	name = "SceneManager";
	this->engine = engine;

	sceneIntro = new SceneIntro(engine);
	AddScene(sceneIntro);
	currentScene = sceneIntro;

	gameTime = 0.0f;
}

M_SceneManager::~M_SceneManager()
{
	CleanUp();
}

bool M_SceneManager::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);


	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Awake();
	}

	return ret;
}


bool M_SceneManager::Start()
{
	bool ret = true;

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Start();
	}
	//Importer::GetInstance()->sceneImporter->Load(engine->GetSceneManager()->GetCurrentScene(), "SceneIntro");

	currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	currentGizmoMode = ImGuizmo::MODE::WORLD;
	return ret;
}

bool M_SceneManager::PreUpdate(float dt)
{
	bool ret = true;

	OPTICK_EVENT();

	PrepareUpdate();
	UpdateGuizmo();

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->PreUpdate(gameDt);
	}

	return ret;
}

bool M_SceneManager::Update(float dt)
{
	bool ret = true;

	OPTICK_EVENT();
	
	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Update(dt);
	}

	return ret;
}

bool M_SceneManager::PostUpdate(float dt)
{
	bool ret = true;

	OPTICK_EVENT();

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->PostUpdate(dt);
	}

	FinishUpdate();
	GuizmoTransformation();
	return ret;
}

bool M_SceneManager::CleanUp()
{
	bool ret = true;
	ImGuizmo::Enable(false);

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->CleanUp();
		RELEASE((*scene));
	}
	scenes.clear();
	scenes.shrink_to_fit();

	return ret;
}

// Method to receive and manage events
void M_SceneManager::OnNotify(const Event& event)
{
	// Manage events
}

bool M_SceneManager::SaveConfiguration(Json& configModule) const
{
	configModule["DefaultScene"] = defaultScene;
	return true;
}

bool M_SceneManager::LoadConfiguration(Json& configModule)
{
	defaultScene = configModule["DefaultScene"];
	return true;
}

bool M_SceneManager::InspectorDraw()
{
	if (ImGui::CollapsingHeader("Scene Manager##"))
	{
		ImGui::InputText("Default Scene: ", &defaultScene);
	}
	return true;
}

bool M_SceneManager::PrepareUpdate()
{
	bool ret = true;

	if (runtimeState == GameState::PLAYING ||
		runtimeState == GameState::TICK)
	{
		frameCount++;
		time += timer.ReadSec();
		gameDt = timer.ReadSec() * gameClockSpeed;
		gameTime += gameDt;
		timer.Start();
	}

	return ret;
}

bool M_SceneManager::FinishUpdate()
{
	bool ret = true;

	if (runtimeState == GameState::TICK)
		OnPause();

	return ret;
}

void M_SceneManager::AddScene(Scene* scene)
{
	scene->Init();
	scenes.push_back(scene);
}

Scene* M_SceneManager::GetCurrentScene()
{
	return currentScene;
}

GameState M_SceneManager::GetGameState()
{
	return runtimeState;
}

void M_SceneManager::OnPlay()
{
	runtimeState = GameState::PLAYING;
	gameClockSpeed = timeScale;

	gameTime = 0.0f;

	// Serialize scene and save it as a .json
	Importer::GetInstance()->sceneImporter->Save(currentScene);

	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnPlay();
	}
}

void M_SceneManager::OnPause()
{
	runtimeState = GameState::PAUSED;
	gameClockSpeed = 0.0f;

	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnPause();
	}
}

void M_SceneManager::OnStop()
{
	runtimeState = GameState::STOPPED;
	gameClockSpeed = 0.0f;
	frameCount = 0;
	time = 0.0f;
	gameTime = 0.0f;

	Importer::GetInstance()->sceneImporter->Load(currentScene,currentScene->name.c_str());
	// Load the scene we saved before in .json
	//LoadScene(currentScene, "SceneIntro");
	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnStop();
	}
}

void M_SceneManager::OnResume()
{
	runtimeState = GameState::PLAYING;
	gameClockSpeed = timeScale;

	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnResume();
	}
}

void M_SceneManager::OnTick()
{
	runtimeState = GameState::TICK;
	gameClockSpeed = timeScale;

	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnTick();
	}
}

void M_SceneManager::OnClick(SDL_Event event)
{
}

void M_SceneManager::GuizmoTransformation()
{
	GameObject* selectedGameObject = currentScene->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID);
	if (!selectedGameObject->GetComponent<C_Transform>()) return;

	if (selectedGameObject == nullptr || selectedGameObject->GetUID() == -1)
		return;

	float4x4 viewMatrix = engine->GetCamera3D()->currentCamera->cameraFrustum.ViewMatrix();
	viewMatrix.Transpose();

	float4x4 projectionMatrix = engine->GetCamera3D()->currentCamera->cameraFrustum.ProjectionMatrix().Transposed();
	float4x4 modelProjection = selectedGameObject->GetComponent<C_Transform>()->GetGlobalTransform().Transposed();

	window = ImGui::FindWindowByName("Scene");
	ImGuizmo::SetDrawlist(window->DrawList);

	ImGuizmo::SetRect(engine->GetEditor()->scenePanelOrigin.x , engine->GetEditor()->scenePanelOrigin.y , engine->GetEditor()->viewportSize.x, engine->GetEditor()->viewportSize.y);

	float tempTransform[16];
	memcpy(tempTransform, modelProjection.ptr(), 16 * sizeof(float));
	
	ImGuizmo::MODE finalMode = (currentGizmoOperation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : currentGizmoMode);
	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), currentGizmoOperation, finalMode, tempTransform);

	if (ImGuizmo::IsUsing())
	{
		float4x4 newTransform;
		newTransform.Set(tempTransform);
		modelProjection = newTransform.Transposed();
		selectedGameObject->GetComponent<C_Transform>()->SetGlobalTransform(modelProjection);
	}
}

void M_SceneManager::UpdateGuizmo()
{
	GameObject* selectedGameObject = currentScene->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID);
	if (selectedGameObject != nullptr && selectedGameObject->GetComponent<C_Camera>() != nullptr && currentGizmoOperation == ImGuizmo::OPERATION::SCALE)
		currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

	if (engine->GetEditor()->MouseOnScene() && (engine->GetInput()->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT))
	{

		if ((engine->GetInput()->GetKey(SDL_SCANCODE_W) == KEY_DOWN))
		{
			currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			CONSOLE_LOG("Set Guizmo to Translate");
		}
		if ((engine->GetInput()->GetKey(SDL_SCANCODE_E) == KEY_DOWN))
		{
			currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
			CONSOLE_LOG("Set Guizmo to Rotate");
		}
		if (selectedGameObject != nullptr && selectedGameObject->GetComponent<C_Camera>() == nullptr)
		{
			if ((engine->GetInput()->GetKey(SDL_SCANCODE_R) == KEY_DOWN))
			{
				currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
				CONSOLE_LOG("Set Guizmo to Scale");
			}
		}
	
	}
}