#include "M_SceneManager.h"
#include "Engine.h"
#include "M_Editor.h"
#include "SceneIntro.h"
#include "M_Camera3D.h"
#include "M_Window.h"
#include "M_ResourceManager.h"
#include <imgui_stdlib.h>

#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Animator.h"
#include "C_Info.h"
#include "C_Camera.h"
#include "C_Script.h"
#include "AnimatorClip.h"

// UI
#include "C_Canvas.h"
#include "C_Transform2D.h"
#include "C_Image.h"
#include "C_Button.h"
#include "C_Text.h"

#include "R_Material.h"
#include "R_Texture.h"
#include "R_Model.h"
#include "R_Animation.h"

#include "PanelViewport.h"
#include "Log.h"
#include "Scripting.h"
#include "Importer.h"

#include "Log.h"
#include "Globals.h"
#include "FSDefs.h"

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

bool M_SceneManager::LoadResourceToScene(Resource* resource)
{
	if (resource == nullptr)
	{
		KOFI_ERROR(" Scene: Could not load resource into scene, resource pointer was nullptr.");
		return false;
	}

	bool ret = false;
	switch (resource->GetType())
	{
	case ResourceType::MODEL:
		ret = CreateGameObjectsFromModel((R_Model*)resource);
		break;
	case ResourceType::TEXTURE:
		ret = ApplyTextureToSelectedGameObject(resource->GetUID());
		break;
	default:
		CONSOLE_LOG("[WARNING] Scene: Could not load resource into scene, resource type not accepted by drag and drop.");
		break;
	}

	return ret;
}

bool M_SceneManager::ApplyTextureToSelectedGameObject(UID uid)
{
	if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.empty())
	{
		KOFI_ERROR(" Scene: Could not apply texture to selected GameObject(s), no GameObject was selected.");
		return false;
	}
	if (uid == 0)
	{
		KOFI_ERROR(" Scene: Could not apply texture to selected GameObject(s), texture uid was 0.");
		return false;
	}

	R_Texture* texture = (R_Texture*)engine->GetResourceManager()->RequestResource(uid);
	if (texture == nullptr)
	{
		KOFI_ERROR(" Scene: Could not apply texture to selected GameObject(s), resource texture was nullptr.");
		return false;
	}

	for (const auto& i : engine->GetEditor()->panelGameObjectInfo.selectedGameObjects)
	{
		GameObject* go = currentScene->GetGameObject(i);
		C_Material* cMaterial = go->GetComponent<C_Material>();

		if (cMaterial == nullptr)
			continue;

		cMaterial->texture = texture;
	}

	return true;
}

bool M_SceneManager::CreateGameObjectsFromModel(R_Model* model)
{
	if (model == nullptr)
	{
		KOFI_ERROR(" Scene Manager: Could not generate GameObjects from resource model, model was nullptr.");
		return false;
	}
	// TODO: Add sceneModels map

	std::map<UID, GameObject*> tmp;
	GameObject* modelRoot = nullptr;

	// nodes
	for (const auto& node : model->nodes)
	{
		GameObject* go = new GameObject(node.uid, engine, node.name.c_str());

		currentScene->rootGo->AttachChild(go);
		go->SetParentUID(node.parentUid);
		go->GetComponent<C_Transform>()->SetPosition(node.position);
		go->GetComponent<C_Transform>()->SetRotationQuat(node.rotation);
		go->GetComponent<C_Transform>()->SetScale(node.scale);

		CreateComponentsFromNode(model, node, go);

		if (node.parentUid == 0)
		{
			modelRoot = go;
			//std::pair<UID, std::string> modelResource = { model->GetUID(),model->GetAssetPath() };
			//currentScene->sceneModels.emplace(modelRoot->GetUID(), modelResource);
		}
		tmp.emplace(go->GetUID(), go);
	}

	//first = old UID (repeated)
	//second = new UID
	std::map<UID,UID> repeatedUIDs;

	//save repeated UIDs
	for (const auto& it : tmp)
	{
		if (currentScene->GetGameObject(it.first) != nullptr)
		{
			it.second->SetUID(RNG::GetRandomUint());
			repeatedUIDs.emplace(it.first,it.second->GetUID());
		}
		currentScene->gameObjectList.push_back(it.second);
	}

	// Reparenting & update gameobjects with repeated UIDs
	for (const auto& it : tmp)
	{
		UID parentUid = it.second->GetParentUID();

		auto newParentUid = repeatedUIDs.find(parentUid);
		if (newParentUid != repeatedUIDs.end())
			it.second->SetParentUID(newParentUid->second);

		if (parentUid == 0 && modelRoot != nullptr)
			it.second->SetParentUID(0);
		else
		{
			std::map<UID, GameObject*>::iterator parent = tmp.find(parentUid);
			if (parent != tmp.end())
			{
				it.second->SetParentUID(parent->second->GetUID());
				parent->second->AttachChild(it.second);
			}
		}
		it.second->GetComponent<C_Transform>()->SetDirty(true);
	}

	// Adding the animator component to the proper game objects.
	bool hasAnimatedMeshes = false;
	for (GameObject* go : modelRoot->GetChildren())
	{
		C_Mesh* cMesh = go->GetComponent<C_Mesh>();
		if (cMesh != nullptr)
		{
			if (cMesh->GetMesh()->IsAnimated())
			{
				hasAnimatedMeshes = true;
				break;
			}
		}
	}
	if (hasAnimatedMeshes) // If the game object contains other game objects with animated meshes...
	{
		// Animation
		if (model->animation != 0 && model->animationName != "")
		{
			C_Animator* cAnimator = (C_Animator*)modelRoot->AddComponentByType(ComponentType::ANIMATOR);
			RELEASE(cAnimator->animation);

			R_Animation* rAnimation = (R_Animation*)engine->GetResourceManager()->RequestResource(model->animation);
			if (cAnimator != nullptr && rAnimation != nullptr)
			{
				for (GameObject* go : modelRoot->GetChildren())
				{
					C_Mesh* cMesh = go->GetComponent<C_Mesh>();
					if (cMesh != nullptr)
					{
						R_Mesh* rMesh = cMesh->GetMesh();
						if (rMesh->IsAnimated())
						{
							//rMesh->SetAnimation(rAnimation);

							// Adding a reference to the mesh for the component cAnimator to be aware of it.
							cAnimator->SetMeshTransforms(rMesh);
						}
					}
				}
				cAnimator->SetAnimation(rAnimation);

				// Updating default clip with all the keyframes of the animation.
				AnimatorClip* animClip = cAnimator->GetSelectedClip();
				animClip->SetStartFrame(0);
				animClip->SetEndFrame(rAnimation->duration);
				animClip->SetDuration(((float)(animClip->GetEndFrame() - animClip->GetStartFrame())) / 1.0f);
				animClip->SetDurationInSeconds(animClip->GetDuration() / rAnimation->GetTicksPerSecond());
			}
		}
	}

	engine->GetResourceManager()->FreeResource(model->GetUID());

	return true;
}

void M_SceneManager::CreateComponentsFromNode(R_Model* model, ModelNode node, GameObject* gameobject)
{
	// Mesh
	if (node.mesh != 0)
	{
		C_Mesh* mesh = (C_Mesh*)gameobject->AddComponentByType(ComponentType::MESH);
		R_Mesh* rMesh = (R_Mesh*)engine->GetResourceManager()->RequestResource(node.mesh);
		if (rMesh == nullptr)
		{
			KOFI_ERROR(" Scene: Could not get resource mesh from model node.");
			gameobject->DeleteComponent(mesh);
			return;
		}
		if (mesh != nullptr)
		{
			mesh->SetMesh(rMesh);
			rMesh->SetRootNode(gameobject->GetParent());
		}

		// Material & Shader
		C_Material* material = (C_Material*)gameobject->AddComponentByType(ComponentType::MATERIAL);
		std::string shaderPath = ASSETS_SHADERS_DIR + std::string("default_shader") + SHADER_EXTENSION;
		R_Material* rMaterial = (R_Material*)engine->GetResourceManager()->GetResourceFromLibrary(shaderPath.c_str());
		material->SetMaterial(rMaterial);

		// Texture
		if (node.texture != 0)
		{
			R_Texture* rTexture = (R_Texture*)engine->GetResourceManager()->RequestResource(node.texture);
			if (rTexture == nullptr)
			{
				KOFI_ERROR(" Scene: Could not get resource texture from model node.");
				return;
			}
			material->texture = rTexture;
			material->checkerTexture = false;
		}
		else
		{
			material->texture = Importer::GetInstance()->textureImporter->GetCheckerTexture();
			material->checkerTexture = true;
		}
	}
}

void M_SceneManager::OnPlay()
{
	runtimeState = GameState::PLAYING;
	frameCount = 0;
	time = 0.0f;
	timer.Start();
	gameClockSpeed = timeScale;
	gameTime = 0.0f;

	// Serialize scene and save it as a .json
	Importer::GetInstance()->sceneImporter->SaveScene(currentScene);

	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnPlay();
	}

	engine->GetRenderer()->ResetFrustumCulling();
	engine->GetCamera3D()->currentCamera->ApplyCullings();
}

void M_SceneManager::OnSceneSwitch()
{
	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnSceneSwitch();
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
	frameCount = 0;
	time = 0.0f;
	gameClockSpeed = 0.0f;
	gameTime = 0.0f;

	Importer::GetInstance()->sceneImporter->LoadScene(currentScene,currentScene->name.c_str());
	// Load the scene we saved before in .json
	//LoadScene(currentScene, "SceneIntro");
	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnStop();
	}

	engine->GetRenderer()->ResetFrustumCulling();
}

void M_SceneManager::OnResume()
{
	runtimeState = GameState::PLAYING;
	timer.Start();
	gameClockSpeed = timeScale;

	for (GameObject* go : currentScene->gameObjectList)
	{
		go->OnResume();
	}
}

void M_SceneManager::OnTick()
{
	runtimeState = GameState::TICK;
	timer.Start();
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
#ifndef KOFI_GAME
	std::vector<GameObject*> selectedGameObjects;
	for (int i = 0; i < engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size(); i++)
		selectedGameObjects.push_back(currentScene->GetGameObject(engine->GetEditor()->engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[i]));

	for (int i = 0; i < selectedGameObjects.size(); i++)
	{
		if (!selectedGameObjects[i]->GetComponent<C_Transform>()) return;

		if (selectedGameObjects[i] == nullptr || selectedGameObjects[i]->GetUID() == 0) return;
	}


	float4x4 viewMatrix = engine->GetCamera3D()->currentCamera->GetCameraFrustum().ViewMatrix();
	viewMatrix.Transpose();

	float4x4 projectionMatrix = engine->GetCamera3D()->currentCamera->GetCameraFrustum().ProjectionMatrix().Transposed();
	ImGuizmo::SetGizmoSizeClipSpace(0.3f);

	std::vector<float4x4> modelProjection;
	for (int i = 0; i < selectedGameObjects.size(); i++)
	{
		modelProjection.push_back(selectedGameObjects[i]->GetComponent<C_Transform>()->GetGlobalTransform().Transposed());
	}

	window = ImGui::FindWindowByName("Scene");

	window->DrawList->PushClipRect(engine->GetEditor()->scenePanelOrigin, ImVec2((engine->GetEditor()->scenePanelOrigin.x + engine->GetEditor()->viewportSize.x), (engine->GetEditor()->scenePanelOrigin.y + engine->GetEditor()->viewportSize.y)), true);
	ImGuizmo::SetDrawlist(window->DrawList);

	ImGuizmo::SetRect(engine->GetEditor()->scenePanelOrigin.x, engine->GetEditor()->scenePanelOrigin.y, engine->GetEditor()->viewportSize.x, engine->GetEditor()->viewportSize.y);

	std::vector<float*> tempTransform;

	for (int i = 0; i < selectedGameObjects.size(); i++)
	{
		tempTransform.push_back((float*)malloc(16 * sizeof(float)));
		memcpy(tempTransform[i], modelProjection[i].ptr(), 16 * sizeof(float));
	}

	ImGuizmo::MODE finalMode = (currentGizmoOperation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : currentGizmoMode);

	for (int i = 0; i < selectedGameObjects.size(); i++)
	{
		ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), currentGizmoOperation, finalMode, tempTransform[i]);
	}

	if (ImGuizmo::IsUsing())
	{
		for (int i = 0; i < selectedGameObjects.size(); i++)
		{
			float4x4 newTransform;
			newTransform.Set(tempTransform[i]);
			modelProjection[i] = newTransform.Transposed();

			selectedGameObjects[i]->GetComponent<C_Transform>()->SetGlobalTransform(modelProjection[i]);
		}

	}
#endif // KOFI_GAME
}

void M_SceneManager::UpdateGuizmo()
{
#ifndef KOFI_GAME
	std::vector<GameObject*> selectedGameObjects;
	for (int i = 0; i < engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size(); i++)
	{
		selectedGameObjects.push_back(currentScene->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[i]));
	}

	for (int i = 0; i < selectedGameObjects.size(); i++)
	{
		if (selectedGameObjects[i] != nullptr && selectedGameObjects[i]->GetComponent<C_Camera>() != nullptr && currentGizmoOperation == ImGuizmo::OPERATION::SCALE)
			currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}

	if (engine->GetEditor()->MouseOnScene() && (engine->GetInput()->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT))
	{
		for (int i = 0; i < selectedGameObjects.size(); i++)
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
			if (selectedGameObjects[i] != nullptr && selectedGameObjects[i]->GetComponent<C_Camera>() == nullptr)
			{
				if ((engine->GetInput()->GetKey(SDL_SCANCODE_R) == KEY_DOWN))
				{
					currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
					CONSOLE_LOG("Set Guizmo to Scale");
				}
			}
		}
	}
#endif // KOFI_GAME
}