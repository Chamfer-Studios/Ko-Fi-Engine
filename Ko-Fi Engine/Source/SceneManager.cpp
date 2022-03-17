#include "SceneManager.h"
#include "Engine.h"
#include "Editor.h"
#include "SceneIntro.h"
#include "Camera3D.h"
#include "Window.h"

#include "GameObject.h"
#include "Material.h"
#include "Texture.h"

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentInfo.h"
#include "ComponentCamera.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "ComponentScript.h"

#include "PanelViewport.h"

#include "Log.h"
#include "Globals.h"


SceneManager::SceneManager(KoFiEngine* engine)
{
	name = "SceneManager";
	this->engine = engine;

	sceneIntro = new SceneIntro(engine);
	AddScene(sceneIntro);
	currentScene = sceneIntro;
}

SceneManager::~SceneManager()
{
	CleanUp();
}

bool SceneManager::Awake()
{
	bool ret = true;

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Awake();
	}

	return ret;
}

bool SceneManager::Start()
{
	bool ret = true;

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Start();
	}
	Importer::GetInstance()->sceneImporter->Load(engine->GetSceneManager()->GetCurrentScene(), "SceneIntro");

	currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	currentGizmoMode = ImGuizmo::MODE::WORLD;
	//ImGuizmo::SetGizmoSizeClipSpace(0.1f);
	return ret;
}

bool SceneManager::PreUpdate(float dt)
{
	bool ret = true;

	PrepareUpdate();
	UpdateGuizmo();

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->PreUpdate(gameDt);
	}

	return ret;
}

bool SceneManager::Update(float dt)
{
	bool ret = true;
	
	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Update(dt);
	}

	return ret;
}

bool SceneManager::PostUpdate(float dt)
{
	bool ret = true;

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->PostUpdate(dt);
	}

	FinishUpdate();
	GuizmoTransformation();
	return ret;
}

bool SceneManager::CleanUp()
{
	bool ret = true;
	ImGuizmo::Enable(false);

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->CleanUp();
		RELEASE((*scene));
	}
	scenes.clear();

	return ret;
}

// Method to receive and manage events
void SceneManager::OnNotify(const Event& event)
{
	// Manage events
}

bool SceneManager::PrepareUpdate()
{
	bool ret = true;

	if (runtimeState == RuntimeState::PLAYING ||
		runtimeState == RuntimeState::TICK)
	{
		frameCount++;
		time += timer.ReadSec();
		gameDt = timer.ReadSec() * gameClockSpeed;
		timer.Start();
	}

	return ret;
}

bool SceneManager::FinishUpdate()
{
	bool ret = true;

	if (runtimeState == RuntimeState::TICK)
		OnPause();

	return ret;
}

void SceneManager::AddScene(Scene* scene)
{
	scene->Init();
	scenes.push_back(scene);
}

Scene* SceneManager::GetCurrentScene()
{
	return currentScene;
}

RuntimeState SceneManager::GetState()
{
	return runtimeState;
}

void SceneManager::OnPlay()
{
	runtimeState = RuntimeState::PLAYING;
	gameClockSpeed = timeScale;

	// Serialize scene and save it as a .json
	Importer::GetInstance()->sceneImporter->Save(currentScene);

	for (GameObject* go : currentScene->gameObjectList)
	{
		ComponentScript* script = go->GetComponent<ComponentScript>();
		if (script != nullptr)
		{
			script->ReloadScript();
		}
	}
}

void SceneManager::OnPause()
{
	runtimeState = RuntimeState::PAUSED;
	gameClockSpeed = 0.0f;
}

void SceneManager::OnStop()
{
	runtimeState = RuntimeState::STOPPED;
	gameClockSpeed = 0.0f;
	frameCount = 0;
	time = 0.0f;

	Importer::GetInstance()->sceneImporter->Load(currentScene,currentScene->name.c_str());
	// Load the scene we saved before in .json
	//LoadScene(currentScene, "SceneIntro");
}

void SceneManager::OnResume()
{
	runtimeState = RuntimeState::PLAYING;
	gameClockSpeed = timeScale;
}

void SceneManager::OnTick()
{
	runtimeState = RuntimeState::TICK;
	gameClockSpeed = timeScale;
}

void SceneManager::OnClick(SDL_Event event)
{
	if (event.button.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT ) return;
	
	if (!engine->GetEditor()->GetPanel<PanelViewport>()->IsWindowFocused() || ImGuizmo::IsOver()) return;
	
	// Mouse Picking
	GameObject* hit = engine->GetCamera3D()->MousePicking();
	if (hit != nullptr)
	{
		engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = hit->GetUID();// Select Object Hit
	}
	else {
		engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = -1; // else, diselect by choosing the root.
	}
}

void SceneManager::GuizmoTransformation()
{
	GameObject* selectedGameObject = currentScene->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID);
	if (selectedGameObject == nullptr || selectedGameObject->GetUID() == -1)
		return;

	float4x4 viewMatrix = engine->GetCamera3D()->viewMatrix.Transposed();
	float4x4 projectionMatrix = engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed();
	float4x4 objectTransform = selectedGameObject->GetComponent<ComponentTransform>()->GetGlobalTransform().Transposed();

	float tempTransform[16];
	memcpy(tempTransform, objectTransform.ptr(), 16 * sizeof(float));


	ImGuizmo::SetRect(engine->GetEditor()->scenePanelOrigin.x , engine->GetEditor()->scenePanelOrigin.y , engine->GetEditor()->viewportSize.x, engine->GetEditor()->viewportSize.y);
	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), currentGizmoOperation, currentGizmoOperation != ImGuizmo::OPERATION::SCALE ? currentGizmoMode : ImGuizmo::MODE::LOCAL, tempTransform);
	
	if (ImGuizmo::IsUsing()) // while using guizmo
	{
		float4x4 newTransform;
		newTransform.Set(tempTransform);
		objectTransform = newTransform.Transposed();
		selectedGameObject->GetComponent<ComponentTransform>()->SetGlobalTransform(objectTransform);


		
	}
}

void SceneManager::UpdateGuizmo()
{
	GameObject* selectedGameObject = currentScene->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID);
	if (selectedGameObject != nullptr && selectedGameObject->GetComponent<ComponentCamera>() != nullptr && currentGizmoOperation == ImGuizmo::OPERATION::SCALE)
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
		if (selectedGameObject != nullptr && selectedGameObject->GetComponent<ComponentCamera>() == nullptr)
		{
			if ((engine->GetInput()->GetKey(SDL_SCANCODE_R) == KEY_DOWN))
			{
				currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
				CONSOLE_LOG("Set Guizmo to Scale");
			}
		}
	}
}
