#include "I_Scene.h"
#include "I_Mesh.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "GameObject.h"

I_Scene::I_Scene()
{
}

I_Scene::~I_Scene()
{
}

bool I_Scene::Import(const char* buffer, uint size, Mesh* mesh)
{
	if (mesh == nullptr)
	{
		LOG("[ERROR] Importer: Could not Import Model! Error: R_Model* was nullptr.");
		return;
	}

	std::string errorString = "[ERROR] Importer: Could not Import Model { " + std::string(mesh->GetAssetFile()) + " }";

	if (buffer == nullptr)
	{
		LOG("%s! Error: Buffer was nullptr.", errorString.c_str());
		return;
	}
	if (size == 0)
	{
		LOG("%s! Error: Size was 0.", errorString.c_str());
		return;
	}

	LOG("[STATUS] Importing Scene: %s", mesh->GetAssetFile());

	const aiScene* assimpScene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (assimpScene == nullptr || assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimpScene->mRootNode)
	{
		LOG("%s! Error: Assimp Error [%s]", errorString.c_str(), aiGetErrorString());
		return;
	}

	for (uint i = 0; i < assimpScene->mNumMeshes; ++i)
	{
		Utilities::aiMeshes.push_back(assimpScene->mMeshes[i]);

		uint matIndex = assimpScene->mMeshes[i]->mMaterialIndex;
		if (matIndex >= 0)
		{
			Utilities::aiMaterials.push_back(assimpScene->mMaterials[matIndex]);
		}
	}

	App->resourceManager->GetForcedUIDsFromMeta(mesh->GetAssetPath(), Utilities::forcedUIDs);				// Getting all the UIDs to force if imported asset already has a .meta file.

	Utilities::CheckAndApplyForcedUID(mesh);																	// Checking if R_Model* has a UID to be forced. Cast rModel to Resource?

	Utilities::ProcessNode(assimpScene, assimpScene->mRootNode, mesh, ModelNode());							// First Parent is empty. Later assigned to scene_root.

	Utilities::ImportAnimations(assimpScene, mesh);

	Utilities::aiMeshes.clear();
	Utilities::aiMaterials.clear();
	Utilities::loadedNodes.clear();
	Utilities::loadedTextures.clear();

	Utilities::forcedUIDs.clear();

	return true;
}


bool I_Scene::Save(const Scene* scene, const char* path)
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
			jsonComponent = component->Save();

			switch (component->GetType())
			{
			case ComponentType::TRANSFORM:
				jsonComponent["component_type"] = "transform";
				break;
			case ComponentType::MESH:
				jsonComponent["component_type"] = "mesh";
				break;
			case ComponentType::MATERIAL:
				jsonComponent["component_type"] = "material";
				break;
			case ComponentType::INFO:
				jsonComponent["component_type"] = "info";
				break;
			case ComponentType::CAMERA:
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



	//bool ret = false;

	//JsonFile sceneJson;
	//// We start setting gameobjects, including the object for the root scene:
	//sceneJson.file["Game Objects"] = json::array(); // Start filling the array for it (as the slides explain)
	//for (std::vector<GameObject*>::iterator goIt = gameObjects.begin(); goIt != gameObjects.end(); goIt++)
	//{
	//	json jsonGO;
	//	jsonGO["name"] = (*goIt)->GetName();
	//	const char* name = (*goIt)->GetName();
	//	jsonGO["active"] = (*goIt)->IsActive();
	//	jsonGO["uuid"] = (*goIt)->GetUUID();
	//	jsonGO["parentUUID"] = (*goIt)->GetParentUUID();

	//	// We start setting its components in another array:
	//	jsonGO["Components"] = json::array();
	//	for (std::vector<Component*>::iterator componentIt = (*goIt)->components.begin(); componentIt != (*goIt)->components.end(); componentIt++)
	//	{
	//		Component* component = (*componentIt);

	//		json jsonComp;
	//		jsonComp["active"] = (*componentIt)->IsActive();
	//		switch ((*componentIt)->GetType())
	//		{
	//		case COMPONENT_TYPE::NONE:
	//			jsonComp["type"] = "NONE";
	//			break;
	//		case COMPONENT_TYPE::TRANSFORM:
	//		{
	//			C_Transform transformComp = *(C_Transform*)component;
	//			transformComp.Save(jsonComp);
	//		}
	//		break;
	//		case COMPONENT_TYPE::MESH:
	//		{
	//			C_Mesh meshComp = *(C_Mesh*)component;
	//			meshComp.Save(jsonComp);

	//			std::string path(MESHES_PATH + std::string(name) + ".DaVMesh");
	//			Importer::Mesh::Save(meshComp.GetMesh(), path.c_str());
	//		}
	//		break;
	//		case COMPONENT_TYPE::MATERIAL:
	//		{
	//			C_Material materialComp = *(C_Material*)component;
	//			materialComp.Save(jsonComp);
	//		}
	//		break;
	//		default:
	//			break;
	//		}
	//		jsonGO["Components"].push_back(jsonComp);
	//	}
	//	sceneJson.file["Game Objects"].push_back(jsonGO);
	//}

	//std::string path = SCENES_PATH + std::string(name) + ".json";
	//ret = sceneJson.Save(path.c_str());

	//return ret;
}


bool I_Scene::Load(const char* path, Scene* mesh)
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
						component->Load(jsonComponent);
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
							component->Load(jsonComponent);
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
						component->Load(jsonComponent);
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


	//bool ret = false;

	//JsonFile sceneJson;
	//std::string path = SCENES_PATH + std::string(nameScene) + ".json";
	//sceneJson.Load(path.c_str());

	//if (!sceneJson.file.is_null())
	//{
	//	for (std::vector<GameObject*>::iterator goIt = gameObjects.begin(); goIt != gameObjects.end(); goIt++)
	//	{
	//		(*goIt)->Clear();
	//		RELEASE((*goIt));
	//	}
	//	app->sceneIntro->sceneGameObjects.clear();
	//	app->sceneIntro->sceneGameObjects.shrink_to_fit();

	//	json jsonGameObjects = sceneJson.file["Game Objects"];

	//	for (auto goIt = jsonGameObjects.begin(); goIt != jsonGameObjects.end(); ++goIt)
	//	{
	//		// We store all values of the different keys in variables to then set the gameobject
	//		std::string name = (*goIt)["name"];
	//		bool active = (*goIt)["active"];
	//		UINT32 uuid = (*goIt)["uuid"];
	//		UINT32 parentUUID = (*goIt)["parentUUID"];

	//		GameObject* gameObj = new GameObject(uuid, active);

	//		if (uuid == 0)
	//		{
	//			gameObj->SetName(nameScene);
	//			gameObj->SetParentUUID(parentUUID);
	//			app->sceneIntro->sceneRoot = gameObj;
	//			app->sceneIntro->selectedGameObj = app->sceneIntro->sceneRoot;
	//		}
	//		else
	//		{
	//			gameObj->SetName(name.c_str());
	//		}

	//		gameObj->SetParentUUID(parentUUID);

	//		json jsonComp = (*goIt)["Components"];
	//		for (auto componentIt = jsonComp.begin(); componentIt != jsonComp.end(); ++componentIt)
	//		{
	//			// We store all values of the different keys as we set the components
	//			bool active = (*componentIt)["active"];
	//			gameObj->transform->SetIsActive(active);

	//			std::string strType = (*componentIt)["type"];

	//			if (strType == "Transform")
	//			{
	//				gameObj->transform->Load(*componentIt);
	//			}
	//			if (strType == "Mesh")
	//			{
	//				C_Mesh* compMesh = (C_Mesh*)gameObj->CreateComponent(COMPONENT_TYPE::MESH);

	//				compMesh->Load(*componentIt);

	//				ret = Importer::Mesh::Load(compMesh->GetMeshPath(), compMesh->GetMesh());
	//			}
	//			if (strType == "Material")
	//			{
	//				C_Material* compMaterial = (C_Material*)gameObj->CreateComponent(COMPONENT_TYPE::MATERIAL);

	//				compMaterial->Load(*componentIt);

	//				ret = Importer::Texture::Import(compMaterial->GetTexturePath(), compMaterial->GetTexture());
	//				Importer::Shader::Import(compMaterial->GetShaderPath(), compMaterial->GetShader());
	//			}
	//		}
	//		gameObjects.push_back(gameObj);
	//	}
	//	for (std::vector<GameObject*>::iterator goIt = gameObjects.begin(); goIt != gameObjects.end(); goIt++)
	//	{
	//		for (std::vector<GameObject*>::iterator childrengoIt = gameObjects.begin(); childrengoIt != gameObjects.end(); childrengoIt++)
	//		{
	//			if ((*childrengoIt)->GetParentUUID() == (*goIt)->GetUUID() && (*childrengoIt)->GetUUID() != 0)
	//			{
	//				(*goIt)->AddChild((*childrengoIt));
	//			}
	//		}
	//	}
	//	ret = true;
	//}
	//else
	//	ret = false;

	//return ret;
}

void I_Scene::ImportNode(const aiScene* assimpScene, const aiNode* assimpNode, Mesh* mesh, const Mesh& parent)
{
	Mesh modelNode = Mesh();
	modelNode.uid = Random::LCG::GetRandomUint();
	modelNode.parentUID = parent.uid;

	assimpNode = Utilities::ImportTransform(assimpNode, modelNode);
	Utilities::ImportMeshesAndMaterials(assimpScene, assimpNode, mesh, modelNode);

	modelNode.name = (assimpNode == assimpScene->mRootNode) ? mesh->GetAssetFile() : assimpNode->mName.C_Str();

	mesh->modelNodes.push_back(modelNode);

	for (uint i = 0; i < assimpNode->mNumChildren; ++i)
	{
		ImportNode(assimpScene, assimpNode->mChildren[i], mesh, modelNode);
	}
}

//void Importer::Scene::Import(const char* path, std::vector<GameObject*>& gameObjects)
//{
//	if (path == nullptr)
//		return;
//
//	char* buffer = nullptr;
//	uint read = app->fileSystem->Load(path, &buffer);
//
//	if (read == 0)
//		return;
//
//	if (buffer == nullptr)
//		return;
//
//	const aiScene* scene = aiImportFileFromMemory((const char*)buffer, read, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
//
//	RELEASE_ARRAY(buffer);
//
//	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//		return;
//
//	Importer::Scene::Private::ProcessNode(scene, scene->mRootNode, gameObjects, gameObjects[0]);
//
//}


//void Importer::Scene::Private::ProcessNode(const aiScene* aiscene, const aiNode* node, std::vector<GameObject*>& gameObjects, GameObject* parent)
//{
//	GameObject* gameObj = new GameObject();
//	gameObj->SetParent(parent);
//	//Import Transform with dummies
//	node = Private::ImportTransform(node, gameObj);
//	Private::ImportMeshesAndMaterial(aiscene, node, gameObj);
//
//	gameObj->SetName(node->mName.C_Str());
//	LOG("ProcessNode node name: %s", gameObj->GetName());
//	gameObjects.push_back(gameObj);
//
//	for (uint i = 0; i < node->mNumChildren; ++i)
//	{
//		Importer::Scene::Private::ProcessNode(aiscene, node->mChildren[i], gameObjects, gameObj);
//	}
//}

//const aiNode* Importer::Scene::Private::ImportTransform(const aiNode* ainode, GameObject* gameObj)
//{
//	aiTransform aiT;
//	mathTransform maT;
//
//	if (strcmp(ainode->mName.C_Str(), "Object010_$AssimpFbx$_Translation") == 0)
//		LOG("AAAA");
//
//	ainode->mTransformation.Decompose(aiT.scale, aiT.rotation, aiT.position);
//
//	maT.position = { aiT.position.x,aiT.position.y, aiT.position.z };
//	maT.rotation = { aiT.rotation.x,aiT.rotation.y, aiT.rotation.z, aiT.rotation.w };
//	maT.scale = { aiT.scale.x,aiT.scale.y, aiT.scale.z };
//
//	while (IsDummyNode(*ainode))
//	{
//		//If node is dummy we add this transform to next dummy until arrives to a node that's not a dummy
//		ainode = ainode->mChildren[0];
//
//		ainode->mTransformation.Decompose(aiT.scale, aiT.rotation, aiT.position);
//
//		mathTransform dummy;
//		dummy.position = { aiT.position.x,aiT.position.y, aiT.position.z };
//		dummy.rotation = { aiT.rotation.x,aiT.rotation.y, aiT.rotation.z, aiT.rotation.w };
//		dummy.scale = { aiT.scale.x,aiT.scale.y, aiT.scale.z };
//
//		maT.position += dummy.position;
//		maT.rotation = maT.rotation * dummy.rotation;
//		maT.scale = { maT.scale.x * aiT.scale.x,maT.scale.y * aiT.scale.y ,maT.scale.z * aiT.scale.z };
//	}
//
//	gameObj->transform->SetPosition(maT.position.x, maT.position.y, maT.position.z);
//	gameObj->transform->SetRotation(maT.rotation.x, maT.rotation.y, maT.rotation.z, maT.rotation.w);
//	gameObj->transform->SetScale(maT.scale.x, maT.scale.y, maT.scale.z);
//
//	return ainode;
//}

//void Importer::Scene::Private::ImportMeshesAndMaterial(const aiScene* aiscene, const aiNode* node, GameObject* gameObj)
//{
//	if (aiscene == nullptr || node == nullptr)
//		return;
//
//	if (!aiscene->HasMeshes())
//	{
//		return;
//	}
//
//	for (uint i = 0; i < node->mNumMeshes; ++i)
//	{
//		aiMesh* aimesh = aiscene->mMeshes[node->mMeshes[i]];
//
//		if (aimesh != nullptr && aimesh->HasFaces())
//		{
//			ImportMesh(aimesh, gameObj, node->mName.C_Str());
//
//			ImportMaterial(aimesh, aiscene, gameObj);
//		}
//	}
//}

//void Importer::Scene::Private::ImportMesh(const aiMesh* aimesh, GameObject* gameObj, const char* name)
//{
//	R_Mesh* rmesh = new R_Mesh();
//
//	bool res = Importer::Mesh::Import(aimesh, rmesh);
//
//	std::string path(MESHES_PATH + std::string(name) + ".DaVMesh");
//
//	if (res)
//	{
//		C_Mesh* compMesh = (C_Mesh*)gameObj->CreateComponent(COMPONENT_TYPE::MESH);
//		compMesh->SetMesh(rmesh);
//		compMesh->SetMeshPath(path.c_str());
//	}
//}

//void Importer::Scene::Private::ImportMaterial(const aiMesh* aimesh, const aiScene* aiscene, GameObject* gameObj)
//{
//	if (aimesh->mMaterialIndex >= 0)
//	{
//		R_Material* rmat = new R_Material();
//		aiMaterial* aimat = aiscene->mMaterials[aimesh->mMaterialIndex];
//		bool res = Importer::Material::Import(aimat, rmat);
//		if (res)
//		{
//			C_Material* compMaterial = (C_Material*)gameObj->CreateComponent(COMPONENT_TYPE::MATERIAL);
//			if (compMaterial == nullptr)
//			{
//				compMaterial = gameObj->GetComponent<C_Material>();
//			}
//
//			compMaterial->SetMaterial(rmat);
//
//			//Componet Material Has materials and textures inside.
//			ImportTexture(aimat, compMaterial);
//
//			R_Shader* rshader = new R_Shader();
//			std::string path(ASSETS_SHADERS_PATH + std::string("defaultShader.DavShader"));
//			Importer::Shader::Import(path.c_str(), rshader);
//			compMaterial->SetShader(rshader);
//			compMaterial->SetShaderPath(path.c_str());
//		}
//	}
//}

//void Importer::Scene::Private::ImportTexture(const aiMaterial* aimaterial, C_Material* compMaterial)
//{
//	aiString textPath;
//	if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textPath) == AI_SUCCESS)
//	{
//		std::string textP = textPath.C_Str();
//
//		std::size_t found = textP.find_last_of("/\\");
//
//		textP = textP.substr(found + 1);
//
//		R_Texture* rtexture = new R_Texture();
//
//		std::string finalPath = ASSETS_TEXTURES_PATH;
//
//		finalPath += textP;
//		bool ret = Importer::Texture::Import(finalPath.c_str(), rtexture);
//
//		if (ret)
//		{
//			compMaterial->SetTexture(rtexture);
//			compMaterial->SetTexturePath(finalPath.c_str());
//		}
//	}
//}

//bool Importer::Scene::Private::IsDummyNode(const aiNode& node)
//{
//	return (strstr(node.mName.C_Str(), "_$AssimpFbx$_") != nullptr && node.mNumChildren == 1);
//}