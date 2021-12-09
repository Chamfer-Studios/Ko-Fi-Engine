#include "SceneManager.h"
#include "Engine.h"
#include "SceneIntro.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentInfo.h"
#include "Editor.h"

#include "Defs.h"
#include "JsonHandler.h"

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

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
}

bool SceneManager::Awake()
{
	bool ret = true;

	for (std::list<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Awake();
	}

	return ret;
}

bool SceneManager::Start()
{
	bool ret = true;

	for (std::list<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Start();
	}

	//SaveScene(currentScene);

	return ret;
}

bool SceneManager::PreUpdate(float dt)
{
	bool ret = true;

	PrepareUpdate();

	for (std::list<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->PreUpdate(gameDt);
	}

	return ret;
}

bool SceneManager::Update(float dt)
{
	bool ret = true;

	for (std::list<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->Update(gameDt);
	}

	return ret;
}

bool SceneManager::PostUpdate(float dt)
{
	bool ret = true;

	for (std::list<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->PostUpdate(gameDt);
	}

	FinishUpdate();

	return ret;
}

bool SceneManager::CleanUp()
{
	bool ret = true;

	for (std::list<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->CleanUp();
		RemoveScene((*scene));
	}
	scenes.clear();

	return ret;
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

void SceneManager::RemoveScene(Scene* scene)
{
	scene->CleanUp();
	scenes.remove(scene);
	RELEASE(scene);
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
	SaveScene(currentScene);
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
	LoadScene(currentScene, "SceneIntro");
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

Json SceneManager::SaveComponentTransform(ComponentTransform* componentTransform)
{
	Json jsonComponentTransform;

	if (componentTransform != nullptr)
	{
		jsonComponentTransform["transform_matrix"] = {
			componentTransform->transformMatrix.Col3(0).x,
			componentTransform->transformMatrix.Col3(0).y,
			componentTransform->transformMatrix.Col3(0).z,
			componentTransform->transformMatrix.Col3(1).x,
			componentTransform->transformMatrix.Col3(1).y,
			componentTransform->transformMatrix.Col3(1).z,
			componentTransform->transformMatrix.Col3(2).x,
			componentTransform->transformMatrix.Col3(2).y,
			componentTransform->transformMatrix.Col3(2).z,
			componentTransform->transformMatrix.Col3(3).x,
			componentTransform->transformMatrix.Col3(3).y,
			componentTransform->transformMatrix.Col3(3).z
		};
		jsonComponentTransform["transform_matrix_local"] = {
			componentTransform->transformMatrixLocal.Col3(0).x,
			componentTransform->transformMatrixLocal.Col3(0).y,
			componentTransform->transformMatrixLocal.Col3(0).z,
			componentTransform->transformMatrixLocal.Col3(1).x,
			componentTransform->transformMatrixLocal.Col3(1).y,
			componentTransform->transformMatrixLocal.Col3(1).z,
			componentTransform->transformMatrixLocal.Col3(2).x,
			componentTransform->transformMatrixLocal.Col3(2).y,
			componentTransform->transformMatrixLocal.Col3(2).z,
			componentTransform->transformMatrixLocal.Col3(3).x,
			componentTransform->transformMatrixLocal.Col3(3).y,
			componentTransform->transformMatrixLocal.Col3(3).z
		};

		jsonComponentTransform["is_dirty"] = componentTransform->GetDirty();

		jsonComponentTransform["position"] = {
			componentTransform->GetPosition().x,
			componentTransform->GetPosition().y,
			componentTransform->GetPosition().z
		};
		jsonComponentTransform["rotation"] = {
			componentTransform->GetRotation().x,
			componentTransform->GetRotation().y,
			componentTransform->GetRotation().z
		};
		jsonComponentTransform["scale"] = {
			componentTransform->GetScale().x,
			componentTransform->GetScale().y,
			componentTransform->GetScale().z
		};
	}

	return jsonComponentTransform;
}

Json SceneManager::SaveComponentMesh(ComponentMesh* componentMesh)
{
	Json jsonComponentMesh;

	jsonComponentMesh["path"] = componentMesh->GetPath().c_str();
	jsonComponentMesh["vertex_normals"] = componentMesh->GetVertexNormals();
	jsonComponentMesh["faces_normals"] = componentMesh->GetFacesNormals();

	return jsonComponentMesh;
}

Json SceneManager::SaveComponentMaterial(ComponentMaterial* componentMaterial)
{
	Json jsonComponentMaterial;

	jsonComponentMaterial["path"] = componentMaterial->GetPath();
	Texture texture = componentMaterial->GetTexture();
	jsonComponentMaterial["texture"]["texture_id"] = (int)texture.textureID;
	jsonComponentMaterial["texture"]["width"] = texture.width;
	jsonComponentMaterial["texture"]["height"] = texture.height;
	jsonComponentMaterial["texture"]["nr_channels"] = texture.nrChannels;
	jsonComponentMaterial["texture"]["texture_path"] = texture.texturePath.c_str();

	return jsonComponentMaterial;
}

Json SceneManager::SaveComponentInfo(ComponentInfo* componentInfo)
{
	Json jsonComponentInfo;

	jsonComponentInfo["path"] = componentInfo->GetPath();

	return jsonComponentInfo;
}

// Serialize and save scene into a .json file
bool SceneManager::SaveScene(Scene* scene)
{
	bool ret = false;

	Json jsonFile;

	const char* sceneName = scene->name.GetString();
	jsonFile[sceneName];
	//Json jsonScene = jsonFile.at(sceneName); // It doesn't work :/

	jsonFile[sceneName]["name"] = sceneName;
	jsonFile[sceneName]["active"] = scene->active;

	std::vector<GameObject*> gameObjectList = scene->gameObjectList;
	jsonFile[sceneName]["game_objects_amount"] = gameObjectList.size();
	jsonFile[sceneName]["game_objects_list"] = Json::array();
	//Json jsonGameObjectList = jsonScene.at("game_object_list"); // It doesn't work :/
	for (std::vector<GameObject*>::iterator go = gameObjectList.begin(); go != gameObjectList.end(); go++)
	{
		GameObject* gameObject = (*go);
		Json jsonGameObject;

		std::string idString = std::to_string((int)gameObject->GetId());

		jsonGameObject/*[idString.c_str()]*/["name"] = gameObject->GetName().c_str();
		jsonGameObject/*[idString.c_str()]*/["active"] = gameObject->active;

		jsonGameObject/*[idString.c_str()]*/["component_transform"] = SaveComponentTransform(gameObject->GetComponent<ComponentTransform>());
		
		jsonGameObject/*[idString.c_str()]*/["components_list"] = Json::array();
		std::vector<Component*> componentsList = gameObject->GetComponents();
		for (std::vector<Component*>::iterator cmp = componentsList.begin(); cmp != componentsList.end(); cmp++)
		{
			Component* component = (*cmp);;
			Json jsonComponent;
			switch (component->GetType())
			{
			case ComponentType::TRANSFORM:
				jsonComponent = SaveComponentTransform((ComponentTransform*)component);
				jsonComponent["component_type"] = "transform";
				break;
			case ComponentType::MESH:
				jsonComponent = SaveComponentMesh((ComponentMesh*)component);
				jsonComponent["component_type"] = "mesh";
				break;
			case ComponentType::MATERIAL:
				jsonComponent = SaveComponentMaterial((ComponentMaterial*)component);
				jsonComponent["component_type"] = "material";
				break;
			case ComponentType::INFO:
				jsonComponent = SaveComponentInfo((ComponentInfo*)component);
				jsonComponent["component_type"] = "info";
				break;
			default:
				break;
			}
			jsonGameObject/*[idString.c_str()]*/["components_list"].push_back(jsonComponent);
		}

		// We are just saving a game object...
		// We don't want to save also its children here,
		// because we will arrive and create them at the proper moment with the loop.
		// In order to keep track of parents and childrens, we will record the ids.
		// This way when we load them, we'll be able to create all the game objects,
		// and create afterwards the parent-children relations knowing the IDs.
		jsonGameObject/*[idString.c_str()]*/["children_id_list"] = Json::array();
		std::vector<GameObject*> children = gameObject->GetChildren();
		for (std::vector<GameObject*>::iterator ch = children.begin(); ch != children.end(); ch++)
		{
			GameObject* child = (*ch);
			jsonGameObject/*[idString.c_str()]*/["children_id_list"].push_back((int)child->GetId());
		}

		if (gameObject->GetParent() != nullptr)
			jsonGameObject/*[idString.c_str()]*/["parent"] = (int)gameObject->GetParent()->GetId();
		else
			jsonGameObject/*[idString.c_str()]*/["parent"];

		jsonGameObject/*[idString.c_str()]*/["id"] = (int)gameObject->GetId();

		jsonFile[sceneName]["game_objects_list"].push_back(jsonGameObject);
	}

	jsonFile[sceneName]["root_go"] = (int)scene->rootGo->GetId();

	ret = jsonHandler.SaveJson(jsonFile, "Scenes/scene.json");

	return ret;
}

void SceneManager::LoadComponentTransform(ComponentTransform* componentTransform, Json jsonComponentTransform)
{
	std::vector<float> values = jsonComponentTransform["transform_matrix"].get<std::vector<float>>();
	float3 columnVector;
	for (int i = 0; i < 4; i++)
	{
		columnVector = { values[i * 3], values[(i * 3) + 1], values[(i * 3) + 2] };
		componentTransform->transformMatrix.SetCol3(i, columnVector);
	}

	values = jsonComponentTransform["transform_matrix_local"].get<std::vector<float>>();
	for (int i = 0; i < 4; i++)
	{
		columnVector = { values[i * 3], values[(i * 3) + 1], values[(i * 3) + 2] };
		componentTransform->transformMatrixLocal.SetCol3(i, columnVector);
	}

	componentTransform->SetDirty(jsonComponentTransform.at("is_dirty"));

	values = jsonComponentTransform["position"].get<std::vector<float>>();
	float3 position;
	position.x = values[0];
	position.y = values[1];
	position.z = values[2];
	componentTransform->SetPosition(position);

	values = jsonComponentTransform["rotation"].get<std::vector<float>>();
	float3 rotation;
	rotation.x = values[0];
	rotation.y = values[1];
	rotation.z = values[2];
	componentTransform->SetRotation(rotation);

	values = jsonComponentTransform["scale"].get<std::vector<float>>();
	float3 scale;
	scale.x = values[0];
	scale.y = values[1];
	scale.z = values[2];
	componentTransform->SetScale(scale);
}

void SceneManager::LoadComponentMesh(ComponentMesh* componentMesh, Json jsonComponentMesh)
{
	componentMesh->SetPath(jsonComponentMesh.at("path"));
	componentMesh->SetVertexNormals(jsonComponentMesh.at("vertex_normals"));
	componentMesh->SetFacesNormals(jsonComponentMesh.at("faces_normals"));
}

void SceneManager::LoadComponentMaterial(ComponentMaterial* componentMaterial, Json jsonComponentMaterial)
{
	componentMaterial->SetPath(jsonComponentMaterial.at("path"));
	Json jsonTexture = jsonComponentMaterial.at("texture");
	Texture texture;
	texture.textureID = jsonTexture.at("texture_id");
	texture.width = jsonTexture.at("width");
	texture.height = jsonTexture.at("height");
	texture.nrChannels = jsonTexture.at("nr_channels");
	texture.texturePath = jsonTexture.at("texture_path");
	componentMaterial->SetTexture(texture);
}

void SceneManager::LoadComponentInfo(ComponentInfo* componentInfo, Json jsonComponentInfo)
{
	std::string path = jsonComponentInfo.at("path");
	componentInfo->SetPath(path.c_str());
}

// Load scene from a .json file
bool SceneManager::LoadScene(Scene* scene, const char* sceneName)
{
	bool ret = false;

	Json jsonFile;
	Json jsonScene;
	ret = jsonHandler.LoadJson(jsonFile, "Scenes/scene.json");

	if (!jsonFile.empty())
	{
		ret = true;
		jsonScene = jsonFile.at(sceneName);

		// LOAD HERE THE PATHS TO EACH MODEL WE HAD IN THE SCENE
		// (SAVE THEM IN A LIST EACH TIME A MODEL IS LOADED)
		// AND LOAD THE MODELS TO GENERATE THE GAME OBJECTS.
		// THEN ITERATE THE LIST OF GAME OBJECTS JUST CREATED AND UPDATE
		// THEIR INFO WITH THE .JSON FILE INFO. SAVED BEFORE.

		std::string name = jsonScene.at("name");
		scene->name.Create(name.c_str());
		scene->active = jsonScene.at("active");

		// ITERATE HERE THE GAME OBJECTS LIST AND SEEK THE SAME
		// GAME OBJECT WITH THE SAME ID TO UPDATE ITS PROPERTIES.
		// IF THE GAME OBJECT DOESN'T EXIST, IT'LL BE CREATED.
		Json jsonGameObjectsList = jsonScene.at("game_objects_list");
		for (const auto& go : jsonGameObjectsList.items())
		{
			Json jsonGameObject = go.value();
			int id = jsonGameObject["id"];

			// IF THE GAME OBJECT ALREADY EXISTS, IT'LL BE UPDATED
			if (scene->GetGameObject(id) != nullptr)
			{
				GameObject* gameObject = scene->GetGameObject(id);

				gameObject->name = jsonGameObject.at("name");
				gameObject->active = jsonGameObject.at("active").is_boolean();

				LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);

				// ITERATE HERE THE COMPONENTS LIST AND SEEK THE SAME
				// COMPONENT TO UPDATE ITS PROPERTIES.
				// IF THE COMPONENT DOESN'T EXIST, IT'LL BE CREATED.
				// IN THE FUTURE WE'LL HAVE COMPONENT IDS TO HAVE THE SAME COMPONENT SEVERAL TIMES.
				Json jsonComponentsList = jsonGameObject.at("components_list");
				for (const auto& cmp : jsonComponentsList.items())
				{
					Json jsonComponent = cmp.value();
					std::string componentString = jsonComponent.at("component_type");
					Component* component = nullptr;
					if (componentString == "transform")
						component = gameObject->GetComponent<ComponentTransform>();
					else if (componentString == "mesh")
						component = gameObject->GetComponent<ComponentMesh>();
					else if (componentString == "material")
						component = gameObject->GetComponent<ComponentMaterial>();
					else if (componentString == "info")
						component = gameObject->GetComponent<ComponentInfo>();

					// IF THE COMPONENT ALREADY EXISTS, IT'LL BE UPDATED
					if (component != nullptr)
					{
						switch (component->GetType())
						{
						case ComponentType::TRANSFORM:
							LoadComponentTransform((ComponentTransform*)component, jsonComponent);
							break;
						case ComponentType::MESH:
							LoadComponentMesh((ComponentMesh*)component, jsonComponent);
							break;
						case ComponentType::MATERIAL:
							LoadComponentMaterial((ComponentMaterial*)component, jsonComponent);
							break;
						case ComponentType::INFO:
							LoadComponentInfo((ComponentInfo*)component, jsonComponent);
							break;
						default:
							break;
						}
					}
					// IF THE COMPONENT DOESN'T EXIST, IT'LL BE CREATED
					else
					{
						if (componentString == "transform")
							component = gameObject->CreateComponent<ComponentTransform>();
						else if (componentString == "mesh")
							component = gameObject->CreateComponent<ComponentMesh>();
						else if (componentString == "material")
							component = gameObject->CreateComponent<ComponentMaterial>();
						else if (componentString == "info")
							component = gameObject->CreateComponent<ComponentInfo>();
						gameObject->AddComponent(component);

						if (component != nullptr)
						{
							Json jsonComponent = jsonComponentsList.at(componentString);
							switch (component->GetType())
							{
							case ComponentType::TRANSFORM:
								LoadComponentTransform((ComponentTransform*)component, jsonComponent);
								break;
							case ComponentType::MESH:
								LoadComponentMesh((ComponentMesh*)component, jsonComponent);
								break;
							case ComponentType::MATERIAL:
								LoadComponentMaterial((ComponentMaterial*)component, jsonComponent);
								break;
							case ComponentType::INFO:
								LoadComponentInfo((ComponentInfo*)component, jsonComponent);
								break;
							default:
								break;
							}
						}
					}
				}

				gameObject->SetId(jsonGameObject.at("id"));
			}
			// IF THE GAME OBJECT DOESN'T EXIST, IT'LL BE CREATED
			else
			{
				GameObject* gameObject = scene->CreateEmptyGameObject();

				gameObject->name = jsonGameObject.at("name");
				gameObject->active = jsonGameObject.at("active").is_boolean();

				LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);

				Json jsonComponentsList = jsonGameObject.at("components_list");

				for (const auto& cmp : jsonComponentsList.items())
				{
					Json jsonComponent = cmp.value();
					std::string componentString = jsonComponent.at("component_type");
					Component* component = nullptr;
					if (componentString == "transform")
						component = gameObject->CreateComponent<ComponentTransform>();
					else if (componentString == "mesh")
						component = gameObject->CreateComponent<ComponentMesh>();
					else if (componentString == "material")
						component = gameObject->CreateComponent<ComponentMaterial>();
					else if (componentString == "info")
						component = gameObject->CreateComponent<ComponentInfo>();
					gameObject->AddComponent(component);

					if (component != nullptr)
					{
						Json jsonComponent = jsonComponentsList.at(componentString);
						switch (component->GetType())
						{
						case ComponentType::TRANSFORM:
							LoadComponentTransform((ComponentTransform*)component, jsonComponent);
							break;
						case ComponentType::MESH:
							LoadComponentMesh((ComponentMesh*)component, jsonComponent);
							break;
						case ComponentType::MATERIAL:
							LoadComponentMaterial((ComponentMaterial*)component, jsonComponent);
							break;
						case ComponentType::INFO:
							LoadComponentInfo((ComponentInfo*)component, jsonComponent);
							break;
						default:
							break;
						}
					}
				}

				gameObject->SetId(jsonGameObject.at("id").is_number_integer());
			}
		}

		scene->rootGo = scene->GetGameObject(jsonScene.at("root_go"));
		engine->GetEditor()->panelGameObjectInfo.currentGameObjectID = -1;

		// MAKE SURE THERE ISN'T ANY GAME OBJECT WE DON'T HAVE AT THE .JSON FILE
		std::vector<GameObject*> gameObjectsList = scene->gameObjectList;
		for (std::vector<GameObject*>::iterator go = gameObjectsList.begin(); go != gameObjectsList.end(); go++)
		{
			GameObject* gameObject = (*go);
			bool isGameObjectSaved = false;
			for (const auto& jsonGo : jsonGameObjectsList.items())
			{
				Json jsonGameObject = jsonGo.value();
				int id = jsonGameObject.at("id");
				if ((int)gameObject->GetId() == id)
				{
					isGameObjectSaved = true;
					break;
				}
			}
			if (!isGameObjectSaved)
			{
				gameObjectsList.erase(go);
				gameObject->CleanUp();
				RELEASE(gameObject);
			}
		}

		// ONCE ALL GAME OBJECTS ARE CREATED WE CAN UPDATE THE PARENTS AND CHILDREN RELATIONS.
		// SEE IF THE RELATION THAT THE .JSON STABLISHES IS THE SAME OF THE ONE OF OUR GAME OBJECT
		// AND UPDATE IT IF NECESSARY.
		for (const auto& go : jsonGameObjectsList.items())
		{
			Json jsonGameObject = go.value();
			int id = jsonGameObject["id"];

			Json jsonChildrenList = jsonGameObject.at("children_id_list");
			GameObject* gameObject = scene->GetGameObject(id);
			for (const auto& ch : jsonChildrenList.items())
			{
				int childId = ch.value();
				if (gameObject->HasChildrenWithId(childId))
					continue;
				else
				{
					GameObject* child = scene->GetGameObject(childId);
					gameObject->AttachChild(child);
				}
			}

			if (id != -1) // Check that we are not setting a parent to the root...
			{
				int parentId = jsonGameObject.at("parent");
				if (parentId == (int)gameObject->GetParent()->GetId())
					continue;
				else
				{
					GameObject* parent = scene->GetGameObject(parentId);
					parent->AttachChild(gameObject);
				}
			}
		}

		return ret;
	}
}