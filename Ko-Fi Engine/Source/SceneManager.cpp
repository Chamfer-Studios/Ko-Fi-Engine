#include "SceneManager.h"
#include "Engine.h"
#include "SceneIntro.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentInfo.h"
#include "ComponentCamera.h"
#include "Editor.h"
#include "Camera3D.h"
#include "Log.h"

#include "Defs.h"
#include "JsonHandler.h"
#include "glew.h"
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
		ret = (*scene)->Update(gameDt);
	}

	return ret;
}

bool SceneManager::PostUpdate(float dt)
{
	bool ret = true;

	for (std::vector<Scene*>::iterator scene = scenes.begin(); scene != scenes.end(); scene++)
	{
		ret = (*scene)->PostUpdate(gameDt);
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

void SceneManager::OnClick(SDL_Event event)
{
	if (event.button.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT) return;

	if (!engine->GetEditor()->getIfSceneClicked()) return;


	GameObject* hit = engine->GetCamera3D()->MousePicking();
	int a = 0;

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
	std::string name = componentMesh->GetParent()->name;
	std::string savePath = "Library/Meshes/" + name + ".sugar";
	Importer::GetInstance()->SaveModel(componentMesh->GetMesh(), savePath.c_str());
	jsonComponentMesh["path"] = componentMesh->GetPath().c_str();
	jsonComponentMesh["vertex_normals"] = componentMesh->GetVertexNormals();
	jsonComponentMesh["faces_normals"] = componentMesh->GetFacesNormals();
	jsonComponentMesh["mesh"] = savePath.c_str();
	return jsonComponentMesh;
}

Json SceneManager::SaveComponentMaterial(ComponentMaterial* componentMaterial)
{
	Json jsonComponentMaterial;
	json jsonTex;
	jsonComponentMaterial["textures"] = json::array();
	for (Texture& tex : componentMaterial->GetMaterial().textures) {
		
		jsonTex["path"]=tex.GetTexturePath();
		jsonComponentMaterial["textures"].push_back(jsonTex);
	}


	json jsonUniform;
	for (Uniform* uniform : componentMaterial->GetMaterial().uniforms) {
		
		switch (uniform->type)
		{

		case GL_FLOAT:
		{
			UniformT<float>* uf = (UniformT<float>*)uniform;
			jsonUniform["name"] = uf->name;
			jsonUniform["type"] = uf->type;
			jsonUniform["value"] = uf->value;
		}
		break;
		case GL_FLOAT_VEC2:
		{
			UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
			jsonUniform["name"] = uf2->name;
			jsonUniform["type"] = uf2->type;
			jsonUniform["value"]["x"] = uf2->value.x;
			jsonUniform["value"]["y"] = uf2->value.y;
		}
		break;
		case GL_FLOAT_VEC3:
		{
			UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
			jsonUniform["name"] = uf3->name;
			jsonUniform["type"] = uf3->type;
			jsonUniform["value"]["x"] = uf3->value.x;
			jsonUniform["value"]["y"] = uf3->value.y;
			jsonUniform["value"]["z"] = uf3->value.z;
		}
		break;
		case GL_FLOAT_VEC4:
		{
			UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
			jsonUniform["name"] = uf4->name;
			jsonUniform["type"] = uf4->type;
			jsonUniform["value"]["x"] = uf4->value.x;
			jsonUniform["value"]["y"] = uf4->value.y;
			jsonUniform["value"]["z"] = uf4->value.z;
			jsonUniform["value"]["w"] = uf4->value.w;
		}
		break;
		case GL_INT:
		{
			UniformT<int>* ui = (UniformT<int>*)uniform;
			jsonUniform["name"] = ui->name;
			jsonUniform["type"] = ui->type;
			jsonUniform["value"] = ui->value;
		}
		break;
		}
		jsonComponentMaterial["uniforms"].push_back(jsonUniform);

	}
	jsonComponentMaterial["materialName"] = componentMaterial->GetMaterial().materialName;
	jsonComponentMaterial["materialPath"] = componentMaterial->GetMaterial().materialPath;
	jsonComponentMaterial["shaderPath"] = componentMaterial->GetShaderPath();
	return jsonComponentMaterial;
}

Json SceneManager::SaveComponentInfo(ComponentInfo* componentInfo)
{
	Json jsonComponentInfo;
	return jsonComponentInfo;
}

Json SceneManager::SaveComponentCamera(ComponentCamera* componentCamera)
{
	Json jsonComponentCamera;
	jsonComponentCamera["vertical_fov"] = componentCamera->verticalFOV;
	jsonComponentCamera["near_plane_distance"] = componentCamera->nearPlaneDistance;
	jsonComponentCamera["far_plane_distance"] = componentCamera->farPlaneDistance;
	jsonComponentCamera["draw_frustum"] = componentCamera->drawFrustum;
	jsonComponentCamera["frustum_culling"] = componentCamera->frustumCulling;
	return jsonComponentCamera;
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

		jsonGameObject["name"] = gameObject->GetName().c_str();
		jsonGameObject["active"] = gameObject->active;

		//jsonGameObject["component_transform"] = SaveComponentTransform(gameObject->GetComponent<ComponentTransform>());

		jsonGameObject["components_list"] = Json::array();
		std::vector<Component*> componentsList = gameObject->GetComponents();
		for (std::vector<Component*>::iterator cmp = componentsList.begin(); cmp != componentsList.end(); cmp++)
		{
			Component* component = (*cmp);
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
			case ComponentType::CAMERA:
				jsonComponent = SaveComponentCamera((ComponentCamera*)component);
				jsonComponent["component_type"] = "camera";
			default:
				break;
			}
			jsonGameObject["components_list"].push_back(jsonComponent);
		}

		// We are just saving a game object...
		// We don't want to save also its children here,
		// because we will arrive and create them at the proper moment with the loop.
		// In order to keep track of parents and childrens, we will record the ids.
		// This way when we load them, we'll be able to create all the game objects,
		// and create afterwards the parent-children relations knowing the IDs.
		jsonGameObject["children_id_list"] = Json::array();
		std::vector<GameObject*> children = gameObject->GetChildren();
		for (std::vector<GameObject*>::iterator ch = children.begin(); ch != children.end(); ch++)
		{
			GameObject* child = (*ch);
			jsonGameObject["children_id_list"].push_back((int)child->GetId());
		}

		if (gameObject->GetParent() != nullptr)
			jsonGameObject["parent"] = (int)gameObject->GetParent()->GetId();
		else
			jsonGameObject["parent"];

		jsonGameObject["id"] = (int)gameObject->GetId();

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

void SceneManager::LoadComponentMesh(ComponentMesh* componentMesh, Json jsonComponentsList)
{
	Json jsonComponentMesh;
	Json jsonComponentMaterial;
	for (const auto& cmp : jsonComponentsList.items())
	{
		if (cmp.value().at("component_type") == "mesh")
			jsonComponentMesh = cmp.value();
		else if (cmp.value().at("component_type") == "material")
			jsonComponentMaterial = cmp.value();
	}
	componentMesh->SetPath(jsonComponentMesh.at("path"));
	componentMesh->SetVertexNormals(jsonComponentMesh.at("vertex_normals"));
	componentMesh->SetFacesNormals(jsonComponentMesh.at("faces_normals"));
	Mesh* mesh = new Mesh();
	mesh = Importer::GetInstance()->LoadModel(jsonComponentMesh.at("mesh").get<std::string>().c_str());
	componentMesh->SetMesh(mesh);
	//Loading Material
	if (mesh->tex_coords && !jsonComponentMaterial.empty())
	{
		GameObject* owner = componentMesh->owner;
		ComponentMaterial* cMat = owner->CreateComponent<ComponentMaterial>();
		cMat->GetMaterial().materialName = jsonComponentMaterial.at("materialName").get<std::string>();
		cMat->GetMaterial().materialPath = jsonComponentMaterial.at("materialPath").get<std::string>();
		cMat->LoadShader(jsonComponentMaterial.at("shaderPath").get<std::string>().c_str());
		for (const auto& tex : jsonComponentMaterial.at("textures").items()) {
			cMat->LoadTexture(tex.value().at("path").get<std::string>());
		}
		for (const auto& uni : jsonComponentMaterial.at("uniforms").items()) {
			std::string uniformName = uni.value().at("name").get<std::string>();
			uint uniformType = uni.value().at("type").get<uint>();
			switch (uniformType) {
			case GL_FLOAT:
			{
				UniformT<float>* uniform = (UniformT<float>*)cMat->GetMaterial().FindUniform(uniformName);
				uniform->value = uni.value().at("value");

			}
			break;
			case GL_FLOAT_VEC2:
			{
				UniformT<float2>* uniform = (UniformT<float2>*)cMat->GetMaterial().FindUniform(uniformName);
				uniform->value.x = uni.value().at("value").at("x");
				uniform->value.y = uni.value().at("value").at("y");

			}
			break;
			case GL_FLOAT_VEC3:
			{
				UniformT<float3>* uniform = (UniformT<float3>*)cMat->GetMaterial().FindUniform(uniformName);
				uniform->value.x = uni.value().at("value").at("x");
				uniform->value.y = uni.value().at("value").at("y");
				uniform->value.z = uni.value().at("value").at("z");
			}
			break;
			case GL_FLOAT_VEC4:
			{
				UniformT<float4>* uniform = (UniformT<float4>*)cMat->GetMaterial().FindUniform(uniformName);
				uniform->value.x = uni.value().at("value").at("x");
				uniform->value.y = uni.value().at("value").at("y");
				uniform->value.z = uni.value().at("value").at("z");
				uniform->value.w = uni.value().at("value").at("w");
			}
			break;
			case GL_INT:
			{
				UniformT<int>* uniform = (UniformT<int>*)cMat->GetMaterial().FindUniform(uniformName);
				uniform->value = uni.value().at("value");
			}
			break;
			}

		}
	}
	else if(mesh->tex_coords && jsonComponentMaterial.empty()){
		GameObject* owner = componentMesh->owner;
		ComponentMaterial* cMat = owner->CreateComponent<ComponentMaterial>();
	}
}

void SceneManager::LoadComponentInfo(ComponentInfo* componentInfo, Json jsonComponentInfo)
{
}

void SceneManager::LoadComponentCamera(ComponentCamera* componentCamera, Json jsonComponentCamera)
{
	componentCamera->verticalFOV = jsonComponentCamera.at("vertical_fov");
	componentCamera->nearPlaneDistance = jsonComponentCamera.at("near_plane_distance");
	componentCamera->farPlaneDistance = jsonComponentCamera.at("far_plane_distance");
	componentCamera->drawFrustum = jsonComponentCamera.at("draw_frustum");
	componentCamera->frustumCulling = jsonComponentCamera.at("frustum_culling");
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
				gameObject->active = jsonGameObject.at("active");

				//LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);

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
					else if (componentString == "info")
						component = gameObject->GetComponent<ComponentInfo>();
					else if (componentString == "camera")
						component = gameObject->GetComponent<ComponentCamera>();

					// IF THE COMPONENT ALREADY EXISTS, IT'LL BE UPDATED
					if (component != nullptr)
					{
						switch (component->GetType())
						{
						case ComponentType::TRANSFORM:
							LoadComponentTransform((ComponentTransform*)component, jsonComponent);
							break;
						case ComponentType::MESH:
							LoadComponentMesh((ComponentMesh*)component, jsonComponentsList);
							break;
						case ComponentType::INFO:
							LoadComponentInfo((ComponentInfo*)component, jsonComponent);
							break;
						case ComponentType::CAMERA:
							LoadComponentCamera((ComponentCamera*)component, jsonComponent);
							break;
						default:
							break;
						}
					}
					// IF THE COMPONENT DOESN'T EXIST, IT'LL BE CREATED
					else
					{
						if (componentString == "transform")
							component = gameObject->GetTransform();
						else if (componentString == "mesh")
							component = gameObject->CreateComponent<ComponentMesh>();
						else if (componentString == "info")
							component = gameObject->GetComponent<ComponentInfo>();
						else if (componentString == "camera")
							component = gameObject->CreateComponent<ComponentCamera>();

						if (component != nullptr)
						{
							switch (component->GetType())
							{
							case ComponentType::TRANSFORM:
								LoadComponentTransform((ComponentTransform*)component, jsonComponent);
								break;
							case ComponentType::MESH:
								LoadComponentMesh((ComponentMesh*)component, jsonComponentsList);
								break;
							case ComponentType::INFO:
								LoadComponentInfo((ComponentInfo*)component, jsonComponent);
								break;
							case ComponentType::CAMERA:
								LoadComponentCamera((ComponentCamera*)component, jsonComponent);
							default:
								break;
							}
						}
					}
				}

				gameObject->SetId(id);
			}
			// IF THE GAME OBJECT DOESN'T EXIST, IT'LL BE CREATED
			else
			{
				GameObject* gameObject = scene->CreateEmptyGameObject();

				gameObject->name = jsonGameObject.at("name");
				gameObject->active = jsonGameObject.at("active");

				//LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);

				Json jsonComponentsList = jsonGameObject.at("components_list");
				for (const auto& cmp : jsonComponentsList.items())
				{
					Json jsonComponent = cmp.value();
					std::string componentString = jsonComponent.at("component_type");
					Component* component = nullptr;

					if (componentString == "transform")
						component = gameObject->GetTransform();
					else if (componentString == "mesh")
						component = gameObject->CreateComponent<ComponentMesh>();
					else if (componentString == "info")
						component = gameObject->GetComponent<ComponentInfo>();
					else if (componentString == "camera")
						component = gameObject->CreateComponent<ComponentCamera>();

					if (component != nullptr)
					{
						switch (component->GetType())
						{
						case ComponentType::TRANSFORM:
							LoadComponentTransform(gameObject->GetComponent<ComponentTransform>(), jsonComponent);
							break;
						case ComponentType::MESH:
							LoadComponentMesh((ComponentMesh*)component, jsonComponentsList);
							break;
						case ComponentType::INFO:
							LoadComponentInfo(gameObject->GetComponent<ComponentInfo>(), jsonComponent);
							break;
						case ComponentType::CAMERA:
							LoadComponentCamera((ComponentCamera*)component, jsonComponent);
							break;
						default:
							break;
						}
					}
				}

				gameObject->SetId(id);
			}
		}

		scene->rootGo = scene->GetGameObject(jsonScene.at("root_go"));
		engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = -1;

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
				// IF THE GAME OBJECT IS NOT IN THE .JSON FILE, IT'LL BE REMOVED
				engine->GetSceneManager()->GetCurrentScene()->RemoveGameObjectIterator(go);
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