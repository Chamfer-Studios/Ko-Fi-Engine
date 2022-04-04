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
#include "Navigation.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentInfo.h"
#include "ComponentCamera.h"
#include "C_Collider.h"
#include "ComponentRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentScript.h"
#include "ComponentButton.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentText.h"
#include "ComponentTransform2D.h"
#include "ComponentParticle.h"
#include "ComponentAnimator.h"
#include "ComponentLightSource.h"

#include "C_AudioSource.h"
#include "C_AudioSwitch.h"
#include "ComponentAnimator.h"
#include "ComponentWalkable.h"
#include "ComponentFollowPath.h"

#include "Mesh.h"
#include "Animation.h"
#include "Texture.h"
#include "Material.h"

#include "Importer.h"
#include "I_Mesh.h"
#include "I_Texture.h"
#include "I_Material.h"

#include "AnimatorClip.h"

I_Scene::I_Scene(KoFiEngine* engine) : engine(engine)
{

}

I_Scene::~I_Scene()
{

}

bool I_Scene::Import(const char* path, bool isPrefab)
{
	CONSOLE_LOG("[STATUS] Importer: Importing Scene: %s", path);

	std::string errorString = "[ERROR] Importer: Could not Import Model { " + std::string(path) + " }";

	if (path == nullptr)
		CONSOLE_LOG("[ERROR] Importer: Path is nullptr.");

	const aiScene* assimpScene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	this->assimpScene = (aiScene*)assimpScene;

	if (assimpScene == nullptr || assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimpScene->mRootNode)
	{
		CONSOLE_LOG("[ERROR] Importer: %s! Error: Assimp Error [%s]", errorString.c_str(), aiGetErrorString());
		return false;
	}

	nodeName = Importer::GetInstance()->GetNameFromPath(path);

	ImportNode(assimpScene, assimpScene->mRootNode, engine->GetSceneManager()->GetCurrentScene()->rootGo, isPrefab);

	//ImportAnimations(assimpScene, mesh);

	return true;
}

GameObject* I_Scene::ImportModel(const char* path)
{
	// TODO: WE SHOULD CHANGE THIS
	GameObject* tmp = nullptr;
	return tmp;
}
aiScene* I_Scene::GetAssimpScene()
{
	return assimpScene;
}
void I_Scene::ImportNode(const aiScene* assimpScene, const aiNode* assimpNode, GameObject* parent, bool isPrefab)
{
	GameObject* gameObj = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();

	assimpNode = ImportTransform(assimpNode, gameObj); // THIS IS CAUSING BUGS WITH THE SCALE 
	ImportMeshesAndMaterials(assimpScene, assimpNode, gameObj);

	gameObj->isPrefab = isPrefab;

	nodeName = (assimpNode == assimpScene->mRootNode) ? nodeName : assimpNode->mName.C_Str();
	std::string chainName = nodeName.c_str() ;
	gameObj->SetName(chainName.c_str());
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
	child->GetComponent<ComponentTransform>()->SetRotationQuat(rotation);
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
			ImportMesh(nodeName, assimpMesh, gameObj, assimpScene);

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

void I_Scene::ImportMesh(const char* nodeName, const aiMesh* assimpMesh, GameObject* gameObj, const aiScene* assimpScene)
{
	//std::string assetPath = ASSETS_MODELS_DIR + std::string(nodeName) + MESH_EXTENSION;

	if (assimpMesh == nullptr || gameObj == nullptr)
	{
		return;
	}

	// Import Mesh to GameObject
	Mesh* mesh = new Mesh(Shape::NONE);
	Importer::GetInstance()->meshImporter->Import(assimpMesh, mesh, assimpScene);

	if (mesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Mesh (name: %s) was not imported correctly.", nodeName);
		return;
	}

	ComponentMesh* cMesh = (ComponentMesh*)gameObj->AddComponentByType(ComponentType::MESH);//CreateComponent<ComponentMesh>();
	if (cMesh != nullptr)
		cMesh->SetMesh(mesh);
	else
	{
		CONSOLE_LOG("[ERROR] Component Mesh is nullptr.");
		return;
	}

	if (!assimpScene->HasAnimations())
	{
		CONSOLE_LOG("[WARNING] Scene Importer: Model had no animations to import.");
		return;
	}
	mesh->isAnimated = true;
	Animation* anim = new Animation();
	Importer::GetInstance()->animationImporter->Import(assimpScene->mAnimations[0], anim, assimpScene);

	ComponentAnimator* cAnim = gameObj->CreateComponent<ComponentAnimator>();
	if (cAnim != nullptr)
		cAnim->SetAnim(anim);
	else
	{
		CONSOLE_LOG("[ERROR] Component Animator is nullptr.");
		return;
	}
	
	// Creating a default clip with all the keyframes of the animation.
	AnimatorClip* animClip = new AnimatorClip(anim, "Default clip", 0, anim->duration, 1.0f, true);
	cAnim->CreateDefaultClip(animClip);
}

void I_Scene::ImportMaterial(const char* nodeName, const aiMaterial* assimpMaterial, uint materialIndex, GameObject* gameObj)
{
	if (assimpMaterial == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: aiMaterial is nullptr.");
		return;
	}

	// Import Material to GameObject
	ComponentMaterial* cMaterial = (ComponentMaterial*)gameObj->AddComponentByType(ComponentType::MATERIAL);//CreateComponent<ComponentMaterial>();

	if (cMaterial == nullptr)
	{
		CONSOLE_LOG("[ERROR] Component Material is nullptr.");
		return;
	}

	aiString aiTexturePath;
	std::string texturePath;
	Texture texture;
	//if (aiGetMaterialTexture(assimpMaterial, aiTextureType_DIFFUSE, materialIndex, &aiTexturePath) == AI_SUCCESS)
	if(assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS)
	{
		std::string textureFilename = aiTexturePath.C_Str();

		textureFilename = textureFilename.substr(textureFilename.find_last_of("/\\") + 1);

		texturePath = ASSETS_TEXTURES_DIR + textureFilename;

		texture = Texture();
		bool ret = Importer::GetInstance()->textureImporter->Import(texturePath.c_str(), &texture);

		if (ret)
		{
			//cMaterial->textures.push_back(texture);
			cMaterial->texture = texture;
		}
	}

	Material* material = new Material();

	if (!Importer::GetInstance()->materialImporter->Import(assimpMaterial, material))
	{
		CONSOLE_LOG("[ERROR] Importer: error while importing the material.");
		return;
	}
	else
		cMaterial->SetMaterial(material);

	//if (textureFilename.size() > 0)
	//{
	//	std::string baseFilename = textureFilename.substr(textureFilename.find_last_of("/\\") + 1);
	//	std::string::size_type const p(baseFilename.find_last_of('.'));
	//	std::string filenameWithoutExtension = baseFilename.substr(0, p);
	//	std::string materialPath = ASSETS_MATERIALS_DIR + filenameWithoutExtension + MATERIAL_EXTENSION;
	//	std::string texturePath = ASSETS_TEXTURES_DIR + textureFilename.substr(textureFilename.find_last_of('\\') + 1);
	//	
	//	if (textureFilename.c_str() != nullptr)
	//	{
	//		engine->GetFileSystem()->CreateMaterial(materialPath.c_str(), filenameWithoutExtension.c_str(), texturePath.c_str());
	//		cMaterial->LoadMaterial(materialPath.c_str());
	//	}
	//}
}

bool I_Scene::Save(Scene* scene,const char* customName)
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;

	const char* name = customName == nullptr? scene->name.c_str(): customName;
	std::vector<GameObject*> gameObjectList = scene->gameObjectList;

	jsonFile[name];
	jsonFile[name]["name"] = name;
	jsonFile[name]["active"] = scene->active;
	jsonFile[name]["navmesh"] = Json::object();
	engine->GetNavigation()->Save(jsonFile[name]["navmesh"]);
	jsonFile[name]["game_objects_amount"] = gameObjectList.size()-1;
	jsonFile[name]["game_objects_list"] = Json::array();
	for (std::vector<GameObject*>::iterator goIt = gameObjectList.begin(); goIt != gameObjectList.end(); ++goIt)
	{
		Json jsonGameObject;

		GameObject* gameObject = (*goIt);
		if (gameObject->GetUID() == scene->rootGo->GetUID())
		{
			continue;
		}
		jsonGameObject["name"] = gameObject->GetName();
		jsonGameObject["active"] = gameObject->active;
		jsonGameObject["UID"] = gameObject->GetUID();
		jsonGameObject["is3D"] = gameObject->is3D;
		jsonGameObject["tag"] = (int)gameObject->tag;

		// We don't want to save also its children here.
		// We will arrive and create them when they get here with the loop.
		// So, in order to keep track of parents and childrens, we will record the UID of the parent.
		if (gameObject->GetParent() != nullptr)
			jsonGameObject["parent_UID"] = gameObject->GetParent()->GetUID();
		else
			jsonGameObject["parent_UID"] = gameObject->GetUID();

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
			{
				jsonComponent["type"] = "NONE";
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
			case ComponentType::PARTICLE:
			{
				ComponentParticle* particleCmp = (ComponentParticle*)component;
				particleCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::CAMERA:
			{
				ComponentCamera* cameraCmp = (ComponentCamera*)component;
				cameraCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::COLLIDER2:
			{
				ComponentCollider2* collCmp = (ComponentCollider2*)component;
				collCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::COLLIDER:
			{
				ComponentCollider* collisionCmp = (ComponentCollider*)component;
				collisionCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::SCRIPT:
			{
				ComponentScript* scriptCmp = (ComponentScript*)component;
				scriptCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::RIGID_BODY:
			{
				ComponentRigidBody* rigidBodyCmp = (ComponentRigidBody*)component;
				rigidBodyCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::TRANSFORM2D:
			{
				ComponentTransform2D* transform2DCmp = (ComponentTransform2D*)component;
				transform2DCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::CANVAS:
			{
				ComponentCanvas* canvasCmp = (ComponentCanvas*)component;
				canvasCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::IMAGE:
			{
				ComponentImage* imageCmp = (ComponentImage*)component;
				imageCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::BUTTON:
			{
				ComponentButton* buttonCmp = (ComponentButton*)component;
				buttonCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::TEXT:
			{
				ComponentText* textCmp = (ComponentText*)component;
				textCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::TRANSFORM:
			{
				ComponentTransform* transformCmp = (ComponentTransform*)component;
				transformCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::INFO:
			{
				ComponentInfo* infoCmp = (ComponentInfo*)component;
				infoCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::AUDIO_SOURCE:
			{
				C_AudioSource* audioSrcCmp = (C_AudioSource*)component;
				audioSrcCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::AUDIO_SWITCH:
			{
				C_AudioSwitch* audioSwitchCmp = (C_AudioSwitch*)component;
				audioSwitchCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::ANIMATOR:
			{
				ComponentAnimator* cAnimator = (ComponentAnimator*)component;
				cAnimator->Save(jsonComponent);
			}
			case ComponentType::WALKABLE:
			{
				ComponentWalkable* walkableCmp = (ComponentWalkable*)component;
				walkableCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::FOLLOW_PATH:
			{
				ComponentFollowPath* followCmp = (ComponentFollowPath*)component;
				followCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::LIGHT_SOURCE:
			{
				ComponentLightSource* componentLightSource = (ComponentLightSource*)component;
				componentLightSource->Save(jsonComponent);
				break;
			}
			default:
				break;
			}
			jsonGameObject["components"].push_back(jsonComponent);
		}
		jsonFile[name]["game_objects_list"].push_back(jsonGameObject);
	}
	std::string path = ASSETS_SCENES_DIR + std::string(name) + SCENE_EXTENSION;

	ret = jsonHandler.SaveJson(jsonFile, path.c_str());

	return ret;
}

bool I_Scene::Load(Scene* scene, const char* name)
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;
	Json jsonScene;

	std::string path = ASSETS_SCENES_DIR + std::string(name) + SCENE_EXTENSION;
	ret = jsonHandler.LoadJson(jsonFile, path.c_str());

	if (!jsonFile.is_null())
	{
		scene->DeleteCurrentScene();
		ret = true;
		jsonScene = jsonFile.at(name);
		scene->name = jsonScene.at("name");
		scene->active = jsonScene.at("active");
		//Create Root
		scene->rootGo->SetName(scene->name.c_str());
		if (jsonScene.find("navmesh") != jsonScene.end())
			engine->GetNavigation()->Load(jsonScene.at("navmesh"));

		Json jsonGameObjects = jsonScene.at("game_objects_list");
		float startTime = (float)engine->GetEngineTime();
#pragma omp parallel for
		for (const auto& goIt : jsonGameObjects.items())
		{
			Json jsonGo = goIt.value();
			uint UID = jsonGo.at("UID");
			bool is3D = true;
			if (jsonGo.find("is3D") != jsonGo.end()) {
				is3D = jsonGo.at("is3D");
			}
			Tag tag = Tag::TAG_UNTAGGED;
			if (jsonGo.contains("tag"))
				tag = jsonGo.at("tag");
			GameObject* go = nullptr;
			bool exists = false;

			if (scene->GetGameObject(UID) != nullptr)
			{
				exists = true;
				go = scene->GetGameObject(UID);
				std::string tmp = jsonGo.at("name");
				go->SetName(tmp.c_str());
				go->SetUID(UID);
				go->SetEngine(engine);
				go->is3D = is3D;
			}
			else
			{
				std::string name = jsonGo.at("name");
				go = new GameObject(UID, engine, name.c_str(), is3D);
			}

			go->active = jsonGo.at("active");
			go->tag = tag;
			uint parentUid = jsonGo.at("parent_UID");
			go->SetParentUID(parentUid);

			Json jsonCmp = jsonGo.at("components");
#pragma omp parallel for
			for (const auto& cmpIt : jsonCmp.items())
			{
				Json jsonCmp = cmpIt.value();
				bool active = jsonCmp.at("active");
				std::string type = jsonCmp.at("type");

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
						meshCmp = (ComponentMesh*)go->AddComponentByType(ComponentType::MESH);//CreateComponent<ComponentMesh>();
					}
					meshCmp->active = true;
					meshCmp->Load(jsonCmp);
				}
				else if (type == "material")
				{
					ComponentMaterial* materialCmp = go->GetComponent<ComponentMaterial>();
					if (materialCmp == nullptr)
					{
						materialCmp = (ComponentMaterial*)go->AddComponentByType(ComponentType::MATERIAL);//CreateComponent<ComponentMaterial>();
					}
					materialCmp->active = true;
					materialCmp->Load(jsonCmp);
				}
				else if (type == "info")
				{
					ComponentInfo* infoCmp = (ComponentInfo*)go->AddComponentByType(ComponentType::INFO);//GetComponent<ComponentInfo>();
					infoCmp->active = true;
					infoCmp->Load(jsonCmp); //does nothing as of now
				}
				else if (type == "camera")
				{
					ComponentCamera* cameraCmp = go->GetComponent<ComponentCamera>();
					if (cameraCmp == nullptr)
					{
						cameraCmp = (ComponentCamera*)go->AddComponentByType(ComponentType::CAMERA);//CreateComponent<ComponentCamera>();
					}
					cameraCmp->active = true;
					cameraCmp->Load(jsonCmp);
				}
				else if (type == "script")
				{
					ComponentScript* scriptCmp = go->GetComponent<ComponentScript>();
					if (scriptCmp == nullptr)
					{
						scriptCmp = (ComponentScript*)go->AddComponentByType(ComponentType::SCRIPT);//CreateComponent<ComponentScript>();
					}
					scriptCmp->active = true;
					scriptCmp->Load(jsonCmp);
				}
				else if (type == "transform2D")
				{
					ComponentTransform2D* transform2DCmp = go->GetComponent<ComponentTransform2D>();
					if (transform2DCmp == nullptr)
					{
						transform2DCmp = (ComponentTransform2D*)go->AddComponentByType(ComponentType::TRANSFORM2D);//CreateComponent<ComponentTransform2D>();
					}
					transform2DCmp->active = true;
					transform2DCmp->Load(jsonCmp);
				}
				else if (type == "canvas")
				{
					ComponentCanvas* canvasCmp = go->GetComponent<ComponentCanvas>();
					if (canvasCmp == nullptr)
					{
						canvasCmp = (ComponentCanvas*)go->AddComponentByType(ComponentType::CANVAS);//CreateComponent<ComponentCanvas>();
					}
					canvasCmp->active = true;
					canvasCmp->Load(jsonCmp);
				}
				else if (type == "image")
				{
					ComponentImage* imageCmp = go->GetComponent<ComponentImage>();
					if (imageCmp == nullptr)
					{
						imageCmp = (ComponentImage*)go->AddComponentByType(ComponentType::IMAGE);//CreateComponent<ComponentImage>();
					}
					imageCmp->active = true;
					imageCmp->Load(jsonCmp);
				}
				else if (type == "button")
				{
					ComponentButton* buttonCmp = go->GetComponent<ComponentButton>();
					if (buttonCmp == nullptr)
					{
						buttonCmp = (ComponentButton*)go->AddComponentByType(ComponentType::BUTTON);//CreateComponent<ComponentButton>();
					}
					buttonCmp->active = true;
					buttonCmp->Load(jsonCmp);
				}
				else if (type == "text")
				{
					ComponentText* textCmp = go->GetComponent<ComponentText>();
					if (textCmp == nullptr)
					{
						textCmp = (ComponentText*)go->AddComponentByType(ComponentType::TEXT);//CreateComponent<ComponentText>();
					}
					textCmp->active = true;
					textCmp->Load(jsonCmp);
				}
				else if (type == "rigidBody")
				{
					ComponentRigidBody* rbCmp = go->GetComponent<ComponentRigidBody>();
					if (rbCmp == nullptr)
					{
						rbCmp = (ComponentRigidBody*)go->AddComponentByType(ComponentType::RIGID_BODY);//CreateComponent<ComponentRigidBody>();
					}
					rbCmp->active = true;
					rbCmp->Load(jsonCmp);
				}
				else if (type == "collider2")
				{
					ComponentCollider2* collCmp = go->GetComponent<ComponentCollider2>();
					if (collCmp == nullptr)
					{
						collCmp = new ComponentCollider2(go, ColliderShape::NONE);
					}
					collCmp->active = true;
					collCmp->Load(jsonCmp);
				}
				else if (type == "collider")
				{
					ComponentCollider* colCmp = go->GetComponent<ComponentCollider>();
					if (colCmp == nullptr)
					{
						colCmp = (ComponentCollider*)go->AddComponentByType(ComponentType::COLLIDER);//CreateComponent<ComponentCollider>();
					}
					colCmp->active = true;
					colCmp->Load(jsonCmp);
				}
				else if (type == "particle")
				{
					ComponentParticle* partCmp = go->GetComponent<ComponentParticle>();
					if (partCmp == nullptr)
					{
						partCmp = (ComponentParticle*)go->AddComponentByType(ComponentType::PARTICLE);//CreateComponent<ComponentCollider>();
					}
					partCmp->active = true;
					partCmp->Load(jsonCmp);
				}
				else if (type == "audio_source")
				{
					C_AudioSource* audioSrcCmp = go->GetComponent<C_AudioSource>();
					if (audioSrcCmp == nullptr)
					{
						audioSrcCmp = go->CreateComponent<C_AudioSource>();
					}
					audioSrcCmp->active = true;
					audioSrcCmp->Load(jsonCmp);
				}
				else if (type == "audio_switch")
				{
					C_AudioSwitch* audioSwitchCmp = go->GetComponent<C_AudioSwitch>();
					if (audioSwitchCmp == nullptr)
					{
						audioSwitchCmp = go->CreateComponent<C_AudioSwitch>();
					}
					audioSwitchCmp->active = true;
					audioSwitchCmp->Load(jsonCmp);
				}
				else if (type == "animator")
				{
					ComponentAnimator* cAnimator = go->GetComponent<ComponentAnimator>();
					if (cAnimator == nullptr)
					{
						cAnimator = go->CreateComponent<ComponentAnimator>();
					}
					cAnimator->active = true;
					cAnimator->Load(jsonCmp);
				}
				else if (type == "walkable")
				{
					ComponentWalkable* walCmp = go->GetComponent<ComponentWalkable>();
					if (walCmp == nullptr)
					{
						walCmp = go->CreateComponent<ComponentWalkable>();
					}
					walCmp->active = true;
					walCmp->Load(jsonCmp);
				}
				else if (type == "followPath")
				{
					ComponentFollowPath* follCmp = go->GetComponent<ComponentFollowPath>();
					if (follCmp == nullptr)
					{
						follCmp = go->CreateComponent<ComponentFollowPath>();
					}
					follCmp->active = true;
				}
				else if (type == "lightSource")
				{
					ComponentLightSource* componentLightSource = go->GetComponent<ComponentLightSource>();
					if (componentLightSource == nullptr)
					{
						componentLightSource = (ComponentLightSource*)go->AddComponentByType(ComponentType::LIGHT_SOURCE);
					}
					componentLightSource->active = true;
					componentLightSource->Load(jsonCmp);
				}
			}
			if (!exists)
				scene->gameObjectList.push_back(go);
		}

		float endTime = (float)engine->GetEngineTime();
		appLog->AddLog("Time to load: %f\n", endTime - startTime);

#pragma omp parallel for
		for (std::vector<GameObject*>::iterator goIt = scene->gameObjectList.begin(); goIt < scene->gameObjectList.end(); ++goIt)
		{
			for (std::vector<GameObject*>::iterator childrenIt = scene->gameObjectList.begin(); childrenIt < scene->gameObjectList.end(); ++childrenIt)
			{
				if ((*goIt)->GetUID() == (*childrenIt)->GetParentUID() && (*childrenIt)->GetUID() != -1)
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