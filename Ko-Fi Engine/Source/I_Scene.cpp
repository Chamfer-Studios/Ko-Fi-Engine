#include "I_Scene.h"
#include "Engine.h"
#include "Log.h"
#include "Assimp.h"
#include "MathGeoTransform.h"
#include "FSDefs.h"
#include "JsonHandler.h"
#include "M_Window.h"

#include "Scene.h"
#include "M_SceneManager.h"
#include "M_FileSystem.h"
#include "M_Navigation.h"

#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Info.h"
#include "C_Camera.h"
#include "C_Script.h"
#include "C_Button.h"
#include "C_Canvas.h"
#include "C_Image.h"
#include "C_Text.h"
#include "C_Transform2D.h"
#include "C_Particle.h"
#include "C_Animator.h"
#include "C_LightSource.h"
#include "C_AudioSource.h"
#include "C_AudioSwitch.h"
#include "C_Walkable.h"
#include "C_FollowPath.h"
#include "C_RigidBody.h"
#include "C_BoxCollider.h"
#include "C_SphereCollider.h"
#include "C_CapsuleCollider.h"

#include "R_Mesh.h"
#include "R_Animation.h"
#include "R_Texture.h"
#include "R_Material.h"

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

	std::string errorString = "[ERROR] Importer: Could not Import R_Model { " + std::string(path) + " }";

	if (path == nullptr)
		CONSOLE_LOG("[ERROR] Importer: Path is nullptr.");

	const aiScene* assimpScene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	this->assimpScene = (aiScene*)assimpScene;

	if (assimpScene == nullptr || assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimpScene->mRootNode)
	{
		CONSOLE_LOG("[ERROR] Importer: %s! Error: Assimp Error [%s]", errorString.c_str(), aiGetErrorString());
		return false;
	}

	nodeName = engine->GetFileSystem()->GetNameFromPath(path);

	ImportNode(assimpScene, assimpScene->mRootNode, engine->GetSceneManager()->GetCurrentScene()->rootGo, isPrefab);

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

	child->GetComponent<C_Transform>()->SetPosition(position);
	child->GetComponent<C_Transform>()->SetRotationQuat(rotation);
	child->GetComponent<C_Transform>()->SetScale(scale);

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
	R_Mesh* mesh = new R_Mesh(Shape::NONE);
	Importer::GetInstance()->meshImporter->Import(assimpMesh, mesh, assimpScene);

	if (mesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: R_Mesh (name: %s) was not imported correctly.", nodeName);
		return;
	}

	C_Mesh* cMesh = (C_Mesh*)gameObj->AddComponentByType(ComponentType::MESH);
	if (cMesh != nullptr)
	{
		cMesh->SetMesh(mesh);
		mesh->SetRootNode(gameObj->GetParent());
	}
	else
	{
		CONSOLE_LOG("[ERROR] Component Mesh is nullptr.");
		return;
	}

	if (!assimpScene->HasAnimations())
	{
		CONSOLE_LOG("[WARNING] Scene Importer: R_Model had no animations to import.");
		return;
	}

	R_Animation* anim = new R_Animation();
	Importer::GetInstance()->animationImporter->Import(assimpScene->mAnimations[0], anim);
	mesh->SetIsAnimated(true);
	mesh->SetAnimation(anim);

	C_Animator* cAnim = gameObj->CreateComponent<C_Animator>();
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
	C_Material* cMaterial = (C_Material*)gameObj->AddComponentByType(ComponentType::MATERIAL);//CreateComponent<C_Material>();

	if (cMaterial == nullptr)
	{
		CONSOLE_LOG("[ERROR] Component Material is nullptr.");
		return;
	}

	aiString aiTexturePath;
	std::string texturePath;
	R_Texture *texture;
	//if (aiGetMaterialTexture(assimpMaterial, aiTextureType_DIFFUSE, materialIndex, &aiTexturePath) == AI_SUCCESS)
	if(assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS)
	{
		std::string textureFilename = aiTexturePath.C_Str();

		textureFilename = textureFilename.substr(textureFilename.find_last_of("/\\") + 1);

		texturePath = ASSETS_TEXTURES_DIR + textureFilename;

		texture = new R_Texture();
		bool ret = Importer::GetInstance()->textureImporter->Import(texturePath.c_str(), texture);

		if (ret)
		{
			//cMaterial->textures.push_back(texture);
			cMaterial->texture = texture;
		}
	}

	R_Material* material = new R_Material();

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
		jsonGameObject["is_prefab"] = gameObject->isPrefab;

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
				C_Mesh* meshCmp = (C_Mesh*)component;
				meshCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::MATERIAL:
			{
				C_Material* materialCmp = (C_Material*)component;
				materialCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::PARTICLE:
			{
				C_Particle* particleCmp = (C_Particle*)component;
				particleCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::CAMERA:
			{
				C_Camera* cameraCmp = (C_Camera*)component;
				cameraCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::BOX_COLLIDER:
			{
				C_BoxCollider* boxColCmp = (C_BoxCollider*)component;
				boxColCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::SPHERE_COLLIDER:
			{
				C_SphereCollider* sphereColCmp = (C_SphereCollider*)component;
				sphereColCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::CAPSULE_COLLIDER:
			{
				C_CapsuleCollider* capsuleColCmp = (C_CapsuleCollider*)component;
				capsuleColCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::SCRIPT:
			{
				C_Script* scriptCmp = (C_Script*)component;
				scriptCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::RIGID_BODY:
			{
				C_RigidBody* rigidBodyCmp = (C_RigidBody*)component;
				rigidBodyCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::TRANSFORM2D:
			{
				C_Transform2D* transform2DCmp = (C_Transform2D*)component;
				transform2DCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::CANVAS:
			{
				C_Canvas* canvasCmp = (C_Canvas*)component;
				canvasCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::IMAGE:
			{
				C_Image* imageCmp = (C_Image*)component;
				imageCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::BUTTON:
			{
				C_Button* buttonCmp = (C_Button*)component;
				buttonCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::TEXT:
			{
				C_Text* textCmp = (C_Text*)component;
				textCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::TRANSFORM:
			{
				C_Transform* transformCmp = (C_Transform*)component;
				transformCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::INFO:
			{
				C_Info* infoCmp = (C_Info*)component;
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
				C_Animator* cAnimator = (C_Animator*)component;
				cAnimator->Save(jsonComponent);
				break;
			}
			case ComponentType::WALKABLE:
			{
				C_Walkable* walkableCmp = (C_Walkable*)component;
				walkableCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::FOLLOW_PATH:
			{
				C_FollowPath* followCmp = (C_FollowPath*)component;
				followCmp->Save(jsonComponent);
				break;
			}
			case ComponentType::LIGHT_SOURCE:
			{
				C_LightSource* componentLightSource = (C_LightSource*)component;
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

		engine->GetWindow()->SetTitle("Ko-Fi Engine - " + scene->name);

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
			
			std::string name = jsonGo.at("name");
			GameObject* go = new GameObject(UID, engine, name.c_str(), is3D);
			if (jsonGo.contains("is_prefab"))
				go->isPrefab = jsonGo.at("is_prefab");
			go->active = jsonGo.at("active");
			go->tag = tag;
			uint parentUid = jsonGo.at("parent_UID");
			go->SetParentUID(parentUid);

			Json jsonCmps = jsonGo.at("components");
#pragma omp parallel for
			for (const auto& cmpIt : jsonCmps.items())
			{
				Json jsonCmp = cmpIt.value();
				bool active = jsonCmp.at("active");
				if (jsonCmp.contains("type"))
				{
					std::string type = jsonCmp.at("type");

					if (type == "transform")
					{
						C_Transform* transformCmp = go->GetComponent<C_Transform>();
						transformCmp->active = true;
						transformCmp->Load(jsonCmp);
					}
					else if (type == "mesh")
					{
						C_Mesh* meshCmp = go->GetComponent<C_Mesh>();
						if (meshCmp == nullptr)
						{
							meshCmp = (C_Mesh*)go->AddComponentByType(ComponentType::MESH);
						}
						meshCmp->active = true;
						meshCmp->Load(jsonCmp);
					}
					else if (type == "material")
					{
						C_Material* materialCmp = go->GetComponent<C_Material>();
						if (materialCmp == nullptr)
						{
							materialCmp = (C_Material*)go->AddComponentByType(ComponentType::MATERIAL);
						}
						materialCmp->active = true;
						materialCmp->Load(jsonCmp);
					}
					else if (type == "info")
					{
						C_Info* infoCmp = (C_Info*)go->AddComponentByType(ComponentType::INFO);
						infoCmp->active = true;
						infoCmp->Load(jsonCmp); // Does nothing as of now
					}
					else if (type == "camera")
					{
						C_Camera* cameraCmp = go->GetComponent<C_Camera>();
						if (cameraCmp == nullptr)
						{
							cameraCmp = (C_Camera*)go->AddComponentByType(ComponentType::CAMERA);
						}
						cameraCmp->active = true;
						cameraCmp->Load(jsonCmp);
					}
					else if (type == "script")
					{
						C_Script* scriptCmp = nullptr;
						for (auto c : go->GetComponents()) {
							if (c->type == ComponentType::SCRIPT) {
								int cID = ((C_Script*)c)->id;
								if (jsonCmp.find("id") != jsonCmp.end()) {
									if (cID == jsonCmp.at("id")) {
										scriptCmp = (C_Script*)c;
									}
								}
							}
						}
						if (scriptCmp == nullptr)
						{
							scriptCmp = (C_Script*)go->AddComponentByType(ComponentType::SCRIPT);
						}
						scriptCmp->active = true;
						scriptCmp->Load(jsonCmp);
					}
					else if (type == "transform2D")
					{
						C_Transform2D* transform2DCmp = go->GetComponent<C_Transform2D>();
						if (transform2DCmp == nullptr)
						{
							transform2DCmp = (C_Transform2D*)go->AddComponentByType(ComponentType::TRANSFORM2D);
						}
						transform2DCmp->active = true;
						transform2DCmp->Load(jsonCmp);
					}
					else if (type == "canvas")
					{
						C_Canvas* canvasCmp = go->GetComponent<C_Canvas>();
						if (canvasCmp == nullptr)
						{
							canvasCmp = (C_Canvas*)go->AddComponentByType(ComponentType::CANVAS);
						}
						canvasCmp->active = true;
						canvasCmp->Load(jsonCmp);
					}
					else if (type == "image")
					{
						C_Image* imageCmp = go->GetComponent<C_Image>();
						if (imageCmp == nullptr)
						{
							imageCmp = (C_Image*)go->AddComponentByType(ComponentType::IMAGE);
						}
						imageCmp->active = true;
						imageCmp->Load(jsonCmp);
					}
					else if (type == "button")
					{
						C_Button* buttonCmp = go->GetComponent<C_Button>();
						if (buttonCmp == nullptr)
						{
							buttonCmp = (C_Button*)go->AddComponentByType(ComponentType::BUTTON);
						}
						buttonCmp->active = true;
						buttonCmp->Load(jsonCmp);
					}
					else if (type == "text")
					{
						C_Text* textCmp = go->GetComponent<C_Text>();
						if (textCmp == nullptr)
						{
							textCmp = (C_Text*)go->AddComponentByType(ComponentType::TEXT);
						}
						textCmp->active = true;
						textCmp->Load(jsonCmp);
					}
					else if (type == "rigidBody")
					{
						C_RigidBody* rbCmp = go->GetComponent<C_RigidBody>();
						if (rbCmp == nullptr)
						{
							rbCmp = (C_RigidBody*)go->AddComponentByType(ComponentType::RIGID_BODY);
						}
						rbCmp->active = true;
						rbCmp->Load(jsonCmp);
					}
					else if (type == "boxCollider")
					{
						C_BoxCollider* boxColCmp = go->GetComponent<C_BoxCollider>();
						if (boxColCmp == nullptr)
						{
							boxColCmp = (C_BoxCollider*)go->AddComponentByType(ComponentType::BOX_COLLIDER);
						}
						boxColCmp->active = true;
						boxColCmp->Load(jsonCmp);
					}
					else if (type == "sphereCollider")
					{
						C_SphereCollider* sphereColCmp = go->GetComponent<C_SphereCollider>();
						if (sphereColCmp == nullptr)
						{
							sphereColCmp = (C_SphereCollider*)go->AddComponentByType(ComponentType::SPHERE_COLLIDER);
						}
						sphereColCmp->active = true;
						sphereColCmp->Load(jsonCmp);
					}
					else if (type == "capsuleCollider")
					{
						C_CapsuleCollider* capsuleColCmp = go->GetComponent<C_CapsuleCollider>();
						if (capsuleColCmp == nullptr)
						{
							capsuleColCmp = (C_CapsuleCollider*)go->AddComponentByType(ComponentType::CAPSULE_COLLIDER);
						}
						capsuleColCmp->active = true;
						capsuleColCmp->Load(jsonCmp);
					}
					else if (type == "particle")
					{
						C_Particle* partCmp = go->GetComponent<C_Particle>();
						if (partCmp == nullptr)
						{
							partCmp = (C_Particle*)go->AddComponentByType(ComponentType::PARTICLE);
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
						C_Animator* cAnimator = go->GetComponent<C_Animator>();
						if (cAnimator == nullptr)
						{
							cAnimator = (C_Animator*)go->AddComponentByType(ComponentType::ANIMATOR);
						}
						cAnimator->active = true;
						cAnimator->Load(jsonCmp);
					}
					else if (type == "walkable")
					{
						C_Walkable* walCmp = go->GetComponent<C_Walkable>();
						if (walCmp == nullptr)
						{
							walCmp = go->CreateComponent<C_Walkable>();
						}
						walCmp->active = true;
						walCmp->Load(jsonCmp);
					}
					else if (type == "followPath")
					{
						C_FollowPath* follCmp = go->GetComponent<C_FollowPath>();
						if (follCmp == nullptr)
						{
							follCmp = go->CreateComponent<C_FollowPath>();
						}
						follCmp->active = true;
					}
					else if (type == "lightSource")
					{
						C_LightSource* componentLightSource = go->GetComponent<C_LightSource>();
						if (componentLightSource == nullptr)
						{
							componentLightSource = (C_LightSource*)go->AddComponentByType(ComponentType::LIGHT_SOURCE);
						}
						componentLightSource->active = true;
						componentLightSource->Load(jsonCmp);
					}

					//..
				}
					
			}
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