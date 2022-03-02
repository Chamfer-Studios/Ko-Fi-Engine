#include "I_Scene.h"
#include "Engine.h"
#include "Log.h"
#include "Assimp.h"
#include "MathGeoTransform.h"
#include "FSDefs.h"
#include "JsonHandler.h"

#include "Scene.h"
#include "SceneManager.h"
#include "FileSystem.h"

#include "Mesh.h"
#include "Texture.h"

#include "Importer.h"
#include "I_Mesh.h"
#include "I_Material.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentInfo.h"
#include "ComponentCamera.h"

I_Scene::I_Scene(KoFiEngine* engine) : engine(engine)
{

}

I_Scene::~I_Scene()
{

}

bool I_Scene::Import(const char* path)
{
	CONSOLE_LOG("[STATUS] Importer: Importing Scene: %s", path);

	std::string errorString = "[ERROR] Importer: Could not Import Model { " + std::string(path) + " }";

	if (path == nullptr)
		CONSOLE_LOG("[ERROR] Importer: Path is nullptr.");

	const aiScene* assimpScene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (assimpScene == nullptr || assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimpScene->mRootNode)
	{
		CONSOLE_LOG("[ERROR] Importer: %s! Error: Assimp Error [%s]", errorString.c_str(), aiGetErrorString());
		return false;
	}

	//for (uint i = 0; i < assimpScene->mNumMeshes; ++i)
	//{
	//	aiMeshes.push_back(assimpScene->mMeshes[i]);

	//	uint matIndex = assimpScene->mMeshes[i]->mMaterialIndex;
	//	if (matIndex >= 0)
	//	{
	//		aiMaterials.push_back(assimpScene->mMaterials[matIndex]);
	//	}
	//}

	// Getting all the UIDs to force if imported asset already has a .meta file.
	//App->resourceManager->GetForcedUIDsFromMeta(mesh->GetAssetPath(), Utilities::forcedUIDs);
	
	// Checking if R_Model* has a UID to be forced. Cast rModel to Resource?
	//Utilities::CheckAndApplyForcedUID(mesh);

	ImportNode(assimpScene, assimpScene->mRootNode, engine->GetSceneManager()->GetCurrentScene()->rootGo);

	//Utilities::ImportAnimations(assimpScene, mesh);

	//Utilities::aiMeshes.clear();
	//Utilities::aiMaterials.clear();
	//Utilities::loadedNodes.clear();
	//Utilities::loadedTextures.clear();

	//Utilities::forcedUIDs.clear();

	return true;
}

GameObject* I_Scene::ImportModel(const char* path)
{
	// TODO: WE SHOULD CHANGE THIS
	GameObject* tmp = nullptr;
	return tmp;
}

void I_Scene::ImportNode(const aiScene* assimpScene, const aiNode* assimpNode, GameObject* parent)
{
	GameObject* gameObj = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();

	assimpNode = ImportTransform(assimpNode, gameObj);
	ImportMeshesAndMaterials(assimpScene, assimpNode, gameObj);

	parent->AttachChild(gameObj);

	for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i)
	{
		ImportNode(assimpScene, assimpNode->mChildren[i], gameObj);
	}
}

const aiNode* I_Scene::ImportTransform(const aiNode* assimpNode, GameObject* child)
{
	// Assimp generates dummy nodes to store multiple FBX transformations.
	// All those transformations will be collapsed to the first non-dummy node.

	aiTransform aiT;

	assimpNode->mTransformation.Decompose(aiT.scale, aiT.rotation, aiT.position);

	float3 position = { aiT.position.x, aiT.position.y, aiT.position.z };
	Quat rotation = { aiT.rotation.x, aiT.rotation.y, aiT.rotation.z, aiT.rotation.w };
	float3 scale = { aiT.scale.x, aiT.scale.y, aiT.scale.z };

	while (IsDummyNode(*assimpNode))
	{
		// As dummies will only have one child, selecting the next one to process is easy.
		assimpNode = assimpNode->mChildren[0];

		// Getting the Transform stored in the dummy node.
		assimpNode->mTransformation.Decompose(aiT.scale, aiT.rotation, aiT.position);

		float3 dummyPosition = { aiT.position.x, aiT.position.y, aiT.position.z };
		Quat dummyRotation = { aiT.rotation.x, aiT.rotation.y, aiT.rotation.z, aiT.rotation.w };
		float3 dummyScale = { aiT.scale.x, aiT.scale.y, aiT.scale.z };

		// Adding the dummy's Transform to the current one.
		position += dummyPosition;
		rotation = rotation * dummyRotation;
		scale = { scale.x * dummyScale.x, scale.y * dummyScale.y, scale.z * dummyScale.z };
	}

	child->GetComponent<ComponentTransform>()->SetPosition(position);
	child->GetComponent<ComponentTransform>()->SetRotation(rotation);
	child->GetComponent<ComponentTransform>()->SetScale(scale);

	CONSOLE_LOG("[STATUS] Importer: Imported transforms of node: %s", assimpNode->mName.C_Str());

	return assimpNode;
}

bool I_Scene::IsDummyNode(const aiNode & assimpNode)
{
	// All dummy nodes contain the "_$AssimpFbx$_" string and only one child node.
	return (strstr(assimpNode.mName.C_Str(), "_$AssimpFbx$_") != nullptr && assimpNode.mNumChildren == 1);
}

void I_Scene::ImportMeshesAndMaterials(const aiScene* assimpScene, const aiNode* assimpNode, GameObject* gameObj)
{
	if (assimpScene == nullptr || assimpNode == nullptr || gameObj == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Assimp Scene / Node or GameObject is nullptr.");
		return;
	}
	
	if (!assimpScene->HasMeshes())
	{
		CONSOLE_LOG("[ERROR] Importer: Assimp does not have any Mesh.");
		return;
	}

	const char* nodeName = assimpNode->mName.C_Str();

	for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i)
	{
		aiMesh* assimpMesh = assimpScene->mMeshes[assimpNode->mMeshes[i]];

		if (assimpMesh != nullptr && assimpMesh->HasFaces())
		{
			ImportMesh(nodeName, assimpMesh, gameObj);

			if (assimpScene->HasMaterials() && assimpMesh->mMaterialIndex >= 0)
			{
				aiMaterial* assimpMaterial = assimpScene->mMaterials[assimpMesh->mMaterialIndex];
				ImportMaterial(nodeName, assimpMaterial, assimpMesh->mMaterialIndex, gameObj);
			}
		}
		else
			CONSOLE_LOG("[ERROR] Importer: aiMesh is missing or does not have faces.");
	}
}

void I_Scene::ImportMesh(const char* nodeName, const aiMesh* assimpMesh, GameObject* gameObj)
{
	//std::string assetPath = ASSETS_MODELS_DIR + std::string(nodeName) + MESH_EXTENSION;
	
	if (assimpMesh == nullptr || gameObj == nullptr)
	{
		return;
	}

	// Import Mesh to GameObject
	Mesh* mesh = new Mesh(Shape::NONE);
	Importer::GetInstance()->meshImporter->Import(assimpMesh, mesh);

	if (mesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Mesh (name: %s) was not imported correctly.", nodeName);
		return;
	}

	ComponentMesh* cMesh = gameObj->CreateComponent<ComponentMesh>();
	cMesh->SetMesh(mesh);
}

void I_Scene::ImportMaterial(const char* nodeName, const aiMaterial* assimpMaterial, uint materialIndex, GameObject* gameObj)
{
	//std::string matFullPath = App->fileSystem->GetDirectory(rModel->GetAssetsPath()) + nodeName + MATERIALS_EXTENSION;

	if (assimpMaterial == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: aiMaterial is nullptr.");
		return;
	}

	//for(){
	aiString texturePath;
	aiGetMaterialTexture(assimpMaterial, aiTextureType_DIFFUSE, materialIndex, &texturePath);
	std::string path = texturePath.C_Str();
	Texture t = Texture();
	Importer::GetInstance()->textureImporter->Import(path.c_str(), &t);
	//}
	
	// Import Material to GameObject
	ComponentMaterial* cMaterial = gameObj->CreateComponent<ComponentMaterial>();
	if (path.size() > 0)
	{
		std::string baseFilename = path.substr(path.find_last_of("/\\") + 1);
		std::string::size_type const p(baseFilename.find_last_of('.'));
		std::string filenameWithoutExtension = baseFilename.substr(0, p);
		std::string materialPath = ASSETS_MATERIALS_DIR + filenameWithoutExtension + MATERIAL_EXTENSION;
		std::string texturePath = ASSETS_TEXTURES_DIR + path.substr(path.find_last_of('\\') + 1);
		
		if (path.c_str() != nullptr)
		{
			engine->GetFileSystem()->CreateMaterial(materialPath.c_str(), filenameWithoutExtension.c_str(), texturePath.c_str());
			cMaterial->LoadMaterial(materialPath.c_str());
		}
	}
}

bool I_Scene::Save(Scene* scene)
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;

	const char* name = scene->name.c_str();
	std::vector<GameObject*> gameObjectList = scene->gameObjectList;

	jsonFile[name];
	jsonFile[name]["name"] = name;
	jsonFile[name]["active"] = scene->active;
	jsonFile[name]["game_objects_amount"] = gameObjectList.size();

	jsonFile[name]["game_objects_list"] = Json::array();
	for (std::vector<GameObject*>::iterator goIt = gameObjectList.begin(); goIt != gameObjectList.end(); ++goIt)
	{
		Json jsonGameObject;

		GameObject* gameObject = (*goIt);

		const char* goName = gameObject->GetName();
		jsonGameObject["name"] = goName;
		jsonGameObject["active"] = gameObject->active;
		jsonGameObject["UID"] = gameObject->GetUID();

		// We don't want to save also its children here.
		// We will arrive and create them when they get here with the loop.
		// So, in order to keep track of parents and childrens, we will record the UID of the parent.
		if (gameObject->GetParent() != nullptr)
			jsonGameObject["parent_UID"] = gameObject->GetParent()->GetUID();

		std::vector<Component*> componentsList = gameObject->GetComponents();
		jsonGameObject["components"] = Json::array();
		for (std::vector<Component*>::iterator cmpIt = componentsList.begin(); cmpIt != componentsList.end(); ++cmpIt)
		{
			Json jsonComponent;

			Component* component = (*cmpIt);

			jsonComponent["active"] = component->active;

			switch (component->GetType())
			{
			case ComponentType::NONE:
				jsonComponent["type"] = "NONE";
				break;
			case ComponentType::TRANSFORM:
			{
				ComponentTransform* transformCmp = (ComponentTransform*)component;
				transformCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::MESH:
			{
				ComponentMesh* meshCmp = (ComponentMesh*)component;
				meshCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::MATERIAL:
			{
				ComponentMaterial* materialCmp = (ComponentMaterial*)component;
				materialCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::INFO:
			{
				ComponentInfo* infoCmp = (ComponentInfo*)component;
				infoCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::CAMERA:
			{
				ComponentCamera* cameraCmp = (ComponentCamera*)component;
				cameraCmp->Save(jsonComponent);
				break;
			}
			default:
				break;
			}
			jsonGameObject["components"].push_back(jsonComponent);
		}
		jsonFile[name]["game_objects_list"].push_back(jsonGameObject);
	}
	std::string path = SCENES_DIR + std::string(name) + SCENE_EXTENSION;

	ret = jsonHandler.SaveJson(jsonFile, path.c_str());

	return ret;
}

bool I_Scene::Load(Scene* scene, const char* name)
{
	bool ret = true;
	Json jsonFile;
	JsonHandler jsonHandler;
	std::string path = SCENES_DIR + std::string(name) + SCENE_EXTENSION;
	ret = jsonHandler.LoadJson(jsonFile, path.c_str());

	if (!jsonFile.is_null())
	{
		//for (std::vector<GameObject*>::iterator goIt = gameObjects.begin(); goIt != gameObjects.end(); ++goIt)
		//{
		//	(*goIt)->CleanUp();
		//	RELEASE((*goIt));
		//}

		Json jsonGameObjects = jsonFile["game_objects_list"];
		for (const auto& goIt : jsonGameObjects.items())
		{
			Json jsonGo = goIt.value();

			std::string name = jsonGo["name"];
			bool active = jsonGo["active"];
			uint UID = jsonGo["UID"];
			uint parentUid = jsonGo["parent_UID"];
			GameObject* go = new GameObject(UID, engine, name.c_str());
			go->SetParentUID(parentUid);
			go->active = active;

			Json jsonCmp = jsonGo["components"];
			for (const auto& cmpIt : jsonCmp.items())
			{
				Json jsonCmp = cmpIt.value();
				bool active = jsonCmp["active"];
				std::string type = jsonCmp["type"];

				if (type == "transform")
				{
					ComponentTransform* transformCmp = go->GetComponent<ComponentTransform>();
					transformCmp->active = true;
					transformCmp->Load(jsonCmp);
				}
				else if (type == "mesh")
				{
					ComponentMesh* meshCmp = go->GetComponent<ComponentMesh>();
					if (meshCmp == nullptr)
					{
						meshCmp = go->CreateComponent<ComponentMesh>();
					}
					meshCmp->active = true;
					meshCmp->Load(jsonCmp);
				}
				else if (type == "material")
				{
					ComponentMaterial* materialCmp = go->GetComponent<ComponentMaterial>();
					if (materialCmp == nullptr)
					{
						materialCmp = go->CreateComponent<ComponentMaterial>();
					}
					materialCmp->active = true;
					materialCmp->Load(jsonCmp);
				}
				else if (type == "info")
				{
					ComponentInfo* infoCmp = go->GetComponent<ComponentInfo>();
					infoCmp->active = true;
					infoCmp->Load(jsonCmp);
				}
				else if (type == "camera")
				{
					ComponentCamera* cameraCmp = go->GetComponent<ComponentCamera>();
					if (cameraCmp == nullptr)
					{
						cameraCmp = go->CreateComponent<ComponentCamera>();
					}
					cameraCmp->active = true;
					cameraCmp->Load(jsonCmp);
				}
			}
			scene->gameObjectList.push_back(go);
		}

		for (std::vector<GameObject*>::iterator goIt = scene->gameObjectList.begin(); goIt < scene->gameObjectList.end(); ++goIt)
		{
			for (std::vector<GameObject*>::iterator childrenIt = scene->gameObjectList.begin(); childrenIt < scene->gameObjectList.end(); ++childrenIt)
			{
				if ((*goIt)->GetParentUID() == (*childrenIt)->GetUID() && (*childrenIt)->GetUID() != -1)
				{
					(*goIt)->AttachChild((*childrenIt));
				}
			}
		}
		ret = true;
	}
	else
		ret = false;
	return ret;
}
	//if (!jsonFile.empty())
	//{
	//	ret = true;
	//	jsonScene = jsonFile.at(sceneName);

	//	// LOAD HERE THE PATHS TO EACH MODEL WE HAD IN THE SCENE
	//	// (SAVE THEM IN A LIST EACH TIME A MODEL IS LOADED)
	//	// AND LOAD THE MODELS TO GENERATE THE GAME OBJECTS.
	//	// THEN ITERATE THE LIST OF GAME OBJECTS JUST CREATED AND UPDATE
	//	// THEIR INFO WITH THE .JSON FILE INFO. SAVED BEFORE.

	//	std::string name = jsonScene.at("name");
	//	scene->name = name.c_str();
	//	scene->active = jsonScene.at("active");

	//	// ITERATE HERE THE GAME OBJECTS LIST AND SEEK THE SAME
	//	// GAME OBJECT WITH THE SAME ID TO UPDATE ITS PROPERTIES.
	//	// IF THE GAME OBJECT DOESN'T EXIST, IT'LL BE CREATED.
	//	Json jsonGameObjectsList = jsonScene.at("game_objects_list");
	//	for (const auto& go : jsonGameObjectsList.items())
	//	{
	//		Json jsonGameObject = go.value();
	//		int id = jsonGameObject["id"];

	//		// IF THE GAME OBJECT ALREADY EXISTS, IT'LL BE UPDATED
	//		if (scene->GetGameObject(id) != nullptr)
	//		{
	//			GameObject* gameObject = scene->GetGameObject(id);

	//			gameObject->name = jsonGameObject.at("name");
	//			gameObject->active = jsonGameObject.at("active");

	//			//LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);

	//			// ITERATE HERE THE COMPONENTS LIST AND SEEK THE SAME
	//			// COMPONENT TO UPDATE ITS PROPERTIES.
	//			// IF THE COMPONENT DOESN'T EXIST, IT'LL BE CREATED.
	//			// IN THE FUTURE WE'LL HAVE COMPONENT IDS TO HAVE THE SAME COMPONENT SEVERAL TIMES.
	//			Json jsonComponentsList = jsonGameObject.at("components_list");
	//			for (const auto& cmp : jsonComponentsList.items())
	//			{
	//				Json jsonComponent = cmp.value();
	//				std::string componentString = jsonComponent.at("component_type");
	//				Component* component = nullptr;
	//				if (componentString == "transform")
	//					component = gameObject->GetComponent<ComponentTransform>();
	//				else if (componentString == "mesh")
	//					component = gameObject->GetComponent<ComponentMesh>();
	//				else if (componentString == "info")
	//					component = gameObject->GetComponent<ComponentInfo>();
	//				else if (componentString == "camera")
	//					component = gameObject->GetComponent<ComponentCamera>();

	//				// IF THE COMPONENT ALREADY EXISTS, IT'LL BE UPDATED
	//				if (component != nullptr)
	//				{
	//					component->Load(jsonComponent);
	//				}
	//				// IF THE COMPONENT DOESN'T EXIST, IT'LL BE CREATED
	//				else
	//				{
	//					if (componentString == "transform")
	//						component = gameObject->GetTransform();
	//					else if (componentString == "mesh")
	//						component = gameObject->CreateComponent<ComponentMesh>();
	//					else if (componentString == "info")
	//						component = gameObject->GetComponent<ComponentInfo>();
	//					else if (componentString == "camera")
	//						component = gameObject->CreateComponent<ComponentCamera>();

	//					if (component != nullptr)
	//					{
	//						component->Load(jsonComponent);
	//					}
	//				}
	//			}

	//			gameObject->SetId(id);
	//		}
	//		// IF THE GAME OBJECT DOESN'T EXIST, IT'LL BE CREATED
	//		else
	//		{
	//			GameObject* gameObject = scene->CreateEmptyGameObject();

	//			gameObject->name = jsonGameObject.at("name");
	//			gameObject->active = jsonGameObject.at("active");

	//			//LoadComponentTransform(gameObject->GetTransform(), jsonGameObject["component_transform"]);

	//			Json jsonComponentsList = jsonGameObject.at("components_list");
	//			for (const auto& cmp : jsonComponentsList.items())
	//			{
	//				Json jsonComponent = cmp.value();
	//				std::string componentString = jsonComponent.at("component_type");
	//				Component* component = nullptr;

	//				if (componentString == "transform")
	//					component = gameObject->GetTransform();
	//				else if (componentString == "mesh")
	//					component = gameObject->CreateComponent<ComponentMesh>();
	//				else if (componentString == "info")
	//					component = gameObject->GetComponent<ComponentInfo>();
	//				else if (componentString == "camera")
	//					component = gameObject->CreateComponent<ComponentCamera>();

	//				if (component != nullptr)
	//				{
	//					component->Load(jsonComponent);
	//				}
	//			}

	//			gameObject->SetId(id);
	//		}
	//	}

	//	scene->rootGo = scene->GetGameObject(jsonScene.at("root_go"));
	//	engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = -1;

	//	// MAKE SURE THERE ISN'T ANY GAME OBJECT WE DON'T HAVE AT THE .JSON FILE
	//	std::vector<GameObject*> gameObjectsList = scene->gameObjectList;
	//	for (std::vector<GameObject*>::iterator go = gameObjectsList.begin(); go != gameObjectsList.end(); go++)
	//	{
	//		GameObject* gameObject = (*go);
	//		bool isGameObjectSaved = false;
	//		for (const auto& jsonGo : jsonGameObjectsList.items())
	//		{
	//			Json jsonGameObject = jsonGo.value();
	//			int id = jsonGameObject.at("id");
	//			if ((int)gameObject->GetId() == id)
	//			{
	//				isGameObjectSaved = true;
	//				break;
	//			}
	//		}
	//		if (!isGameObjectSaved)
	//		{
	//			// IF THE GAME OBJECT IS NOT IN THE .JSON FILE, IT'LL BE REMOVED
	//			engine->GetSceneManager()->GetCurrentScene()->RemoveGameObjectIterator(go);
	//		}
	//	}

	//	// ONCE ALL GAME OBJECTS ARE CREATED WE CAN UPDATE THE PARENTS AND CHILDREN RELATIONS.
	//	// SEE IF THE RELATION THAT THE .JSON STABLISHES IS THE SAME OF THE ONE OF OUR GAME OBJECT
	//	// AND UPDATE IT IF NECESSARY.
	//	for (const auto& go : jsonGameObjectsList.items())
	//	{
	//		Json jsonGameObject = go.value();
	//		int id = jsonGameObject["id"];

	//		Json jsonChildrenList = jsonGameObject.at("children_id_list");
	//		GameObject* gameObject = scene->GetGameObject(id);
	//		for (const auto& ch : jsonChildrenList.items())
	//		{
	//			int childId = ch.value();
	//			if (gameObject->HasChildrenWithId(childId))
	//				continue;
	//			else
	//			{
	//				GameObject* child = scene->GetGameObject(childId);
	//				gameObject->AttachChild(child);
	//			}
	//		}

	//		if (id != -1) // Check that we are not setting a parent to the root...
	//		{
	//			int parentId = jsonGameObject.at("parent");
	//			if (parentId == (int)gameObject->GetParent()->GetId())
	//				continue;
	//			else
	//			{
	//				GameObject* parent = scene->GetGameObject(parentId);
	//				parent->AttachChild(gameObject);
	//			}
	//		}
	//	}
	//}
