#include "SceneManager.h"
#include "Engine.h"
#include "SceneIntro.h"
#include "GameObject.h"
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
#include "Material.h"
#include "Texture.h"
#include "Editor.h"
#include "Camera3D.h"
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

	return ret;
}

bool SceneManager::PreUpdate(float dt)
{
	bool ret = true;

	PrepareUpdate();

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

	return ret;
}

bool SceneManager::CleanUp()
{
	bool ret = true;

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
	if (event.button.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT) return;

	if (!engine->GetEditor()->GetPanel<PanelViewport>()->IsWindowFocused()) return;

	GameObject* hit = engine->GetCamera3D()->MousePicking();
	if (hit != nullptr)
	{
		CONSOLE_LOG("%s", hit->GetName());
		engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = hit->GetUID();
	}
	else {
		engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = -1;
	}

}
//Json SceneManager::SaveComponentCanvas(ComponentCanvas* componentCanvas)
//{
//	Json jsonComponentTransform2D;
//	jsonComponentTransform2D["position"] = {
//		componentCanvas->GetPosition().x,
//		componentCanvas->GetPosition().y,
//	};
//	jsonComponentTransform2D["rotation"] = {
//		componentCanvas->GetRotation().x,
//		componentCanvas->GetRotation().y,
//		componentCanvas->GetRotation().z,
//	};
//	jsonComponentTransform2D["size"] = {
//		componentCanvas->GetSize().x,
//		componentCanvas->GetSize().y
//	};
//	jsonComponentTransform2D["pivot"] = {
//		componentCanvas->GetPivot().x,
//		componentCanvas->GetPivot().y
//	};
//	jsonComponentTransform2D["logicalSize"] = {
//		componentCanvas->GetLogicalSize().x,
//		componentCanvas->GetLogicalSize().y
//	};
//	jsonComponentTransform2D["anchor"] = (int)componentCanvas->GetAnchor();
//
//	return jsonComponentTransform2D;
//}
//
//Json SceneManager::SaveComponentTransform2D(ComponentTransform2D* componentTransform2D)
//{
//	Json jsonComponentTransform2D;
//	jsonComponentTransform2D["position"] = {
//		componentTransform2D->GetPosition().x,
//		componentTransform2D->GetPosition().y,
//	};
//	jsonComponentTransform2D["rotation"] = {
//		componentTransform2D->GetRotation().x,
//		componentTransform2D->GetRotation().y,
//		componentTransform2D->GetRotation().z,
//	};
//	jsonComponentTransform2D["size"] = {
//		componentTransform2D->GetSize().x,
//		componentTransform2D->GetSize().y
//	};
//	jsonComponentTransform2D["pivot"] = {
//		componentTransform2D->GetPivot().x,
//		componentTransform2D->GetPivot().y
//	};
//	jsonComponentTransform2D["anchor"] = (int)componentTransform2D->GetAnchor();
//
//	return jsonComponentTransform2D;
//}
//
//Json SceneManager::SaveComponentImage(ComponentImage* componentImage)
//{
//	Json jsonComponentImage;
//
//	jsonComponentImage["texture"] = componentImage->openGLTexture.GetTexturePath();
//	jsonComponentImage["mask"] = {
//		componentImage->GetMask().x,
//		componentImage->GetMask().y
//	};
//
//	return jsonComponentImage;
//}
//
//Json SceneManager::SaveComponentButton(ComponentButton* componentButton)
//{
//	Json jsonComponentButton;
//	jsonComponentButton["idleTexture"] = componentButton->GetIdleTexture().GetTexturePath();
//	jsonComponentButton["hoverTexture"] = componentButton->GetHoverTexture().GetTexturePath();
//	jsonComponentButton["pressedTexture"] = componentButton->GetPressedTexture().GetTexturePath();
//
//	return jsonComponentButton;
//}
//
//Json SceneManager::SaveComponentText(ComponentText* componentText)
//{
//	Json jsonComponentText;
//	jsonComponentText["value"] = componentText->GetTextValue();
//
//	return jsonComponentText;
//}
//
//// Serialize and save scene into a .json file

//void SceneManager::LoadComponentCanvas(ComponentCanvas* componentCanvas, Json jsonComponentCanvas)
//{
//	std::vector<float> values = jsonComponentCanvas["position"].get<std::vector<float>>();
//	float2 position;
//	position.x = values[0];
//	position.y = values[1];
//
//	componentCanvas->SetPosition(position);
//
//	values = jsonComponentCanvas["rotation"].get<std::vector<float>>();
//	float3 rotation;
//	rotation.x = values[0];
//	rotation.y = values[1];
//	rotation.z = values[2];
//
//	componentCanvas->SetRotation(rotation);
//
//	values = jsonComponentCanvas["size"].get<std::vector<float>>();
//	float2 size;
//	size.x = values[0];
//	size.y = values[1];
//
//	componentCanvas->SetSize(size);
//
//	values = jsonComponentCanvas["pivot"].get<std::vector<float>>();
//	float2 pivot;
//	pivot.x = values[0];
//	pivot.y = values[1];
//
//	componentCanvas->SetPivot(pivot);
//
//	values = jsonComponentCanvas["logicalSize"].get<std::vector<float>>();
//	float2 logicalSize;
//	logicalSize.x = values[0];
//	logicalSize.y = values[1];
//
//	componentCanvas->SetLogicalSize(logicalSize);
//
//	int anchor = jsonComponentCanvas["anchor"].get<int>();
//	componentCanvas->SetAnchor((ComponentTransform2D::Anchor)anchor);
//}
//
//void SceneManager::LoadComponentTransform2D(ComponentTransform2D* componentTransform2D, Json jsonComponentTransform2D)
//{
//	std::vector<float> values = jsonComponentTransform2D["position"].get<std::vector<float>>();
//	float2 position;
//	position.x = values[0];
//	position.y = values[1];
//
//	componentTransform2D->SetPosition(position);
//
//	values = jsonComponentTransform2D["rotation"].get<std::vector<float>>();
//	float3 rotation;
//	rotation.x = values[0];
//	rotation.y = values[1];
//	rotation.z = values[2];
//
//	componentTransform2D->SetRotation(rotation);
//
//	values = jsonComponentTransform2D["size"].get<std::vector<float>>();
//	float2 size;
//	size.x = values[0];
//	size.y = values[1];
//
//	componentTransform2D->SetSize(size);
//
//	values = jsonComponentTransform2D["pivot"].get<std::vector<float>>();
//	float2 pivot;
//	pivot.x = values[0];
//	pivot.y = values[1];
//
//	componentTransform2D->SetPivot(pivot);
//
//	int anchor = jsonComponentTransform2D["anchor"].get<int>();
//	componentTransform2D->SetAnchor((ComponentTransform2D::Anchor)anchor);
//}
//
//void SceneManager::LoadComponentImage(ComponentImage* componentImage, Json jsonComponentImage)
//{
//	std::string path = jsonComponentImage["texture"].get<std::string>();
//	componentImage->SetTexture(path.c_str());
//
//	std::vector<float> values = jsonComponentImage["mask"].get<std::vector<float>>();
//	float2 mask;
//	mask.x = values[0];
//	mask.y = values[1];
//}
//
//void SceneManager::LoadComponentButton(ComponentButton* componentButton, Json jsonComponentButton)
//{
//	std::string path = jsonComponentButton["idleTexture"].get<std::string>();
//	componentButton->SetIdleTexture(path.c_str());
//
//	path = jsonComponentButton["hoverTexture"].get<std::string>();
//	componentButton->SetHoverTexture(path.c_str());
//
//	path = jsonComponentButton["pressedTexture"].get<std::string>();
//	componentButton->SetPressedTexture(path.c_str());
//}
//
//void SceneManager::LoadComponentText(ComponentText* componentText, Json jsonComponentText)
//{
//	std::string value = jsonComponentText["value"].get<std::string>();
//	componentText->SetTextValue(value);
//}
//
//// Load scene from a .json file
//bool SceneManager::LoadScene(Scene* scene, const char* sceneName)
//{
//	bool ret = false;
//
//	Json jsonFile;
//	Json jsonScene;
//	ret = jsonHandler.LoadJson(jsonFile, "Scenes/scene.json");
//
//	if (!jsonFile.empty())
//	{
//		ret = true;
//		jsonScene = jsonFile.at(sceneName);
//
//		// LOAD HERE THE PATHS TO EACH MODEL WE HAD IN THE SCENE
//		// (SAVE THEM IN A LIST EACH TIME A MODEL IS LOADED)
//		// AND LOAD THE MODELS TO GENERATE THE GAME OBJECTS.
//		// THEN ITERATE THE LIST OF GAME OBJECTS JUST CREATED AND UPDATE
//		// THEIR INFO WITH THE .JSON FILE INFO. SAVED BEFORE.
//
//		std::string name = jsonScene.at("name");
//		scene->name.Create(name.c_str());
//		scene->active = jsonScene.at("active");
//
//		// ITERATE HERE THE GAME OBJECTS LIST AND SEEK THE SAME
//		// GAME OBJECT WITH THE SAME ID TO UPDATE ITS PROPERTIES.
//		// IF THE GAME OBJECT DOESN'T EXIST, IT'LL BE CREATED.
//		Json jsonGameObjectsList = jsonScene.at("game_objects_list");
//		for (const auto& go : jsonGameObjectsList.items())
//		{
//			Json jsonGameObject = go.value();
//			int id = jsonGameObject["id"];
//
//			// IF THE GAME OBJECT ALREADY EXISTS, IT'LL BE UPDATED
//			if (scene->GetGameObject(id) != nullptr)
//			{
//				GameObject* gameObject = scene->GetGameObject(id);
//
//				gameObject->name = jsonGameObject.at("name");
//				gameObject->active = jsonGameObject.at("active");
//
//				//LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);
//
//				// ITERATE HERE THE COMPONENTS LIST AND SEEK THE SAME
//				// COMPONENT TO UPDATE ITS PROPERTIES.
//				// IF THE COMPONENT DOESN'T EXIST, IT'LL BE CREATED.
//				// IN THE FUTURE WE'LL HAVE COMPONENT IDS TO HAVE THE SAME COMPONENT SEVERAL TIMES.
//				Json jsonComponentsList = jsonGameObject.at("components_list");
//				for (const auto& cmp : jsonComponentsList.items())
//				{
//					Json jsonComponent = cmp.value();
//					std::string componentString = jsonComponent.at("component_type");
//					Component* component = nullptr;
//					if (componentString == "transform")
//						component = gameObject->GetComponent<ComponentTransform>();
//					else if (componentString == "mesh")
//						component = gameObject->GetComponent<ComponentMesh>();
//					else if (componentString == "info")
//						component = gameObject->GetComponent<ComponentInfo>();
//					else if (componentString == "camera")
//						component = gameObject->GetComponent<ComponentCamera>();
//					else if (componentString == "canvas")
//						component = gameObject->GetComponent<ComponentCanvas>();
//					else if (componentString == "transform2d")
//						component = gameObject->GetComponent<ComponentTransform2D>();
//					else if (componentString == "image")
//						component = gameObject->GetComponent<ComponentImage>();
//					else if (componentString == "button")
//						component = gameObject->GetComponent<ComponentButton>();
//					else if (componentString == "text")
//						component = gameObject->GetComponent<ComponentText>();
//
//					// IF THE COMPONENT ALREADY EXISTS, IT'LL BE UPDATED
//					if (component != nullptr)
//					{
//						switch (component->GetType())
//						{
//						case ComponentType::TRANSFORM:
//							LoadComponentTransform((ComponentTransform*)component, jsonComponent);
//							break;
//						case ComponentType::MESH:
//							LoadComponentMesh((ComponentMesh*)component, jsonComponentsList);
//							break;
//						case ComponentType::INFO:
//							LoadComponentInfo((ComponentInfo*)component, jsonComponent);
//							break;
//						case ComponentType::CAMERA:
//							LoadComponentCamera((ComponentCamera*)component, jsonComponent);
//							break;
//						case ComponentType::CANVAS:
//							LoadComponentCanvas((ComponentCanvas*)component, jsonComponent);
//							break;
//						case ComponentType::TRANSFORM2D:
//							LoadComponentTransform2D((ComponentTransform2D*)component, jsonComponent);
//							break;
//						case ComponentType::IMAGE:
//							LoadComponentImage((ComponentImage*)component, jsonComponent);
//							break;
//						case ComponentType::BUTTON:
//							LoadComponentButton((ComponentButton*)component, jsonComponent);
//							break;
//						case ComponentType::TEXT:
//							LoadComponentText((ComponentText*)component, jsonComponent);
//							break;
//						default:
//							break;
//						}
//					}
//					// IF THE COMPONENT DOESN'T EXIST, IT'LL BE CREATED
//					else
//					{
//						if (componentString == "transform")
//							component = gameObject->GetTransform();
//						else if (componentString == "mesh")
//							component = gameObject->CreateComponent<ComponentMesh>();
//						else if (componentString == "info")
//							component = gameObject->GetComponent<ComponentInfo>();
//						else if (componentString == "camera")
//							component = gameObject->CreateComponent<ComponentCamera>();
//						else if (componentString == "canvas")
//							component = gameObject->CreateComponent<ComponentCanvas>();
//						else if (componentString == "transform2d")
//							component = gameObject->CreateComponent<ComponentTransform2D>();
//						else if (componentString == "image")
//							component = gameObject->CreateComponent<ComponentImage>();
//						else if (componentString == "button")
//							component = gameObject->CreateComponent<ComponentButton>();
//						else if (componentString == "text")
//							component = gameObject->CreateComponent<ComponentText>();
//
//						if (component != nullptr)
//						{
//							switch (component->GetType())
//							{
//							case ComponentType::TRANSFORM:
//								LoadComponentTransform((ComponentTransform*)component, jsonComponent);
//								break;
//							case ComponentType::MESH:
//								LoadComponentMesh((ComponentMesh*)component, jsonComponentsList);
//								break;
//							case ComponentType::INFO:
//								LoadComponentInfo((ComponentInfo*)component, jsonComponent);
//								break;
//							case ComponentType::CAMERA:
//								LoadComponentCamera((ComponentCamera*)component, jsonComponent);
//								break;
//							case ComponentType::CANVAS:
//								LoadComponentCanvas((ComponentCanvas*)component, jsonComponent);
//								break;
//							case ComponentType::TRANSFORM2D:
//								LoadComponentTransform2D((ComponentTransform2D*)component, jsonComponent);
//								break;
//							case ComponentType::IMAGE:
//								LoadComponentImage((ComponentImage*)component, jsonComponent);
//								break;
//							case ComponentType::BUTTON:
//								LoadComponentButton((ComponentButton*)component, jsonComponent);
//								break;
//							case ComponentType::TEXT:
//								LoadComponentText((ComponentText*)component, jsonComponent);
//								break;
//							default:
//								break;
//							}
//						}
//					}
//				}
//
//				gameObject->SetId(id);
//			}
//			// IF THE GAME OBJECT DOESN'T EXIST, IT'LL BE CREATED
//			else
//			{
//				bool is3D = (jsonGameObject.contains("is3D") ? jsonGameObject["is3D"].get<bool>() : true);
//				GameObject* gameObject = scene->CreateEmptyGameObject("", is3D);
//
//				gameObject->name = jsonGameObject.at("name");
//				gameObject->active = jsonGameObject.at("active");
//
//				//LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);
//
//				Json jsonComponentsList = jsonGameObject.at("components_list");
//				for (const auto& cmp : jsonComponentsList.items())
//				{
//					Json jsonComponent = cmp.value();
//					std::string componentString = jsonComponent.at("component_type");
//					Component* component = nullptr;
//
//					if (componentString == "transform")
//						component = gameObject->GetTransform();
//					else if (componentString == "mesh")
//						component = gameObject->CreateComponent<ComponentMesh>();
//					else if (componentString == "info")
//						component = gameObject->GetComponent<ComponentInfo>();
//					else if (componentString == "camera")
//						component = gameObject->CreateComponent<ComponentCamera>();
//					else if (componentString == "canvas")
//						component = gameObject->CreateComponent<ComponentCanvas>();
//					else if (componentString == "transform2d")
//						component = gameObject->CreateComponent<ComponentTransform2D>();
//					else if (componentString == "image")
//						component = gameObject->CreateComponent<ComponentImage>();
//					else if (componentString == "button")
//						component = gameObject->CreateComponent<ComponentButton>();
//					else if (componentString == "text")
//						component = gameObject->CreateComponent<ComponentText>();
//
//					if (component != nullptr)
//					{
//						switch (component->GetType())
//						{
//						case ComponentType::TRANSFORM:
//							LoadComponentTransform(gameObject->GetComponent<ComponentTransform>(), jsonComponent);
//							break;
//						case ComponentType::MESH:
//							LoadComponentMesh((ComponentMesh*)component, jsonComponentsList);
//							break;
//						case ComponentType::INFO:
//							LoadComponentInfo(gameObject->GetComponent<ComponentInfo>(), jsonComponent);
//							break;
//						case ComponentType::CAMERA:
//							LoadComponentCamera((ComponentCamera*)component, jsonComponent);
//							break;
//						case ComponentType::CANVAS:
//							LoadComponentCanvas((ComponentCanvas*)component, jsonComponent);
//							break;
//						case ComponentType::TRANSFORM2D:
//							LoadComponentTransform2D((ComponentTransform2D*)component, jsonComponent);
//							break;
//						case ComponentType::IMAGE:
//							LoadComponentImage((ComponentImage*)component, jsonComponent);
//							break;
//						case ComponentType::BUTTON:
//							LoadComponentButton((ComponentButton*)component, jsonComponent);
//							break;
//						case ComponentType::TEXT:
//							LoadComponentText((ComponentText*)component, jsonComponent);
//							break;
//						default:
//							break;
//						}
//					}
//				}
//
//				gameObject->SetId(id);
//			}
//		}
//
//		scene->rootGo = scene->GetGameObject(jsonScene.at("root_go"));
//		engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = -1;
//
//		// MAKE SURE THERE ISN'T ANY GAME OBJECT WE DON'T HAVE AT THE .JSON FILE
//		std::vector<GameObject*> gameObjectsList = scene->gameObjectList;
//		for (std::vector<GameObject*>::iterator go = gameObjectsList.begin(); go != gameObjectsList.end(); go++)
//		{
//			GameObject* gameObject = (*go);
//			bool isGameObjectSaved = false;
//			for (const auto& jsonGo : jsonGameObjectsList.items())
//			{
//				Json jsonGameObject = jsonGo.value();
//				int id = jsonGameObject.at("id");
//				if ((int)gameObject->GetId() == id)
//				{
//					isGameObjectSaved = true;
//					break;
//				}
//			}
//			if (!isGameObjectSaved)
//			{
//				// IF THE GAME OBJECT IS NOT IN THE .JSON FILE, IT'LL BE REMOVED
//				engine->GetSceneManager()->GetCurrentScene()->RemoveGameObjectIterator(go);
//			}
//		}
//
//		// ONCE ALL GAME OBJECTS ARE CREATED WE CAN UPDATE THE PARENTS AND CHILDREN RELATIONS.
//		// SEE IF THE RELATION THAT THE .JSON STABLISHES IS THE SAME OF THE ONE OF OUR GAME OBJECT
//		// AND UPDATE IT IF NECESSARY.
//		for (const auto& go : jsonGameObjectsList.items())
//		{
//			Json jsonGameObject = go.value();
//			int id = jsonGameObject["id"];
//
//			Json jsonChildrenList = jsonGameObject.at("children_id_list");
//			GameObject* gameObject = scene->GetGameObject(id);
//			for (const auto& ch : jsonChildrenList.items())
//			{
//				int childId = ch.value();
//				if (gameObject->HasChildrenWithId(childId))
//					continue;
//				else
//				{
//					GameObject* child = scene->GetGameObject(childId);
//					gameObject->AttachChild(child);
//				}
//			}
//
//			if (id != -1) // Check that we are not setting a parent to the root...
//			{
//				int parentId = jsonGameObject.at("parent");
//				if (parentId == (int)gameObject->GetParent()->GetId())
//					continue;
//				else
//				{
//					GameObject* parent = scene->GetGameObject(parentId);
//					parent->AttachChild(gameObject);
//				}
//			}
//		}
//
//		return ret;
//	}
//}
