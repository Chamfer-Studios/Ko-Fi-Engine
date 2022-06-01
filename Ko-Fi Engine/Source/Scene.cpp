#include "Scene.h"

// Modules
#include "Engine.h"
#include "M_Editor.h"

// GameObject
#include "GameObject.h"
#include "C_LightSource.h"
#include "C_Button.h"

#include <vector>
#include "M_Physics.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h" 
#include "C_Camera.h"
#include "C_RigidBody.h"

GameObject* Scene::GetGameObject(int uid)
{
	for (GameObject* go : gameObjectList)
	{
		if (go->GetUID() == uid)
		{
			return go;
		}
	}
	return nullptr;
}



bool Scene::IsGameObjectInScene(std::string name)
{
	for (GameObject* go : gameObjectList)
	{
		if (go->GetName() == name)
		{
			return true;
		}
	}

	return false;
}
void Scene::OnAnyButtonHovered(const std::function<void()>& onAnyButtonHovered, const std::function<void()>& onNoButtonHovered)
{
	for (GameObject* go : gameObjectList)
	{
		C_Button* cBtn = go->GetComponent<C_Button>();
		if (cBtn)
		{

			if (cBtn->GetState() == C_Button::BUTTON_STATE::HOVER)
			{
				onAnyButtonHovered();
				return;
			}

		}
	}
	onNoButtonHovered();
}

void Scene::OnAnyEnemyHovered(const std::function<void()>& onAnyEnemyHovered, const std::function<void()>& onNoEnemyHovered)
{
	GameObject* hit = engine->GetCamera3D()->MousePicking();
	if (hit != nullptr)
	{
		if (hit->tag == Tag::TAG_ENEMY)
		{
			onAnyEnemyHovered();
			return;
		}
	}
	
	onNoEnemyHovered();
}

void Scene::OnAnySpiceSpotHovered(const std::function<void()>& onAnySpiceSpotHovered, const std::function<void()>& onNoSpiceSpotHovered)
{
	GameObject* hit = engine->GetCamera3D()->MousePicking();
	if (hit != nullptr)
	{
		if (hit->tag == Tag::TAG_PICKUP)
		{
			onAnySpiceSpotHovered();
			return;
		}
	}
	onNoSpiceSpotHovered();
}

void Scene::SwitchCursor(const std::function<void(std::string)>& onChange, const std::function<void()>& onNothingHovered)
{
	//UI HAS PREFERENCE OVER ALL
	for (GameObject* go : gameObjectList)
	{
		C_Button* cBtn = go->GetComponent<C_Button>();
		if (cBtn)
		{
			if (cBtn->GetState() == C_Button::BUTTON_STATE::HOVER)
			{
				std::string path = "Assets/New UI/MouseUI.bmp";
				onChange(path);
				return;
			}
		}
	}

	//then pick up and then enemy
	GameObject* hit = engine->GetCamera3D()->MousePicking();
	if (hit != nullptr)
	{
		if (hit->tag == Tag::TAG_PICKUP)
		{
			std::string path = "Assets/New UI/mousePick.bmp";
			onChange(path);
			return;
		}
		else if (hit->tag == Tag::TAG_ENEMY)
		{
			std::string path = "Assets/New UI/mouseAttack.bmp";
			onChange(path);
			return;
		}
	}

	onNothingHovered();
}

GameObject* Scene::CreateEmptyGameObject(const char* name, GameObject* parent, bool is3D)
{
	GameObject* go = new GameObject(RNG::GetRandomUint(), engine, name, is3D);
	this->gameObjectList.push_back(go);
 	if (parent)
		parent->AttachChild(go);
	else
		this->rootGo->AttachChild(go);

	int GOID = go->GetUID();
	//engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = GOID;

	return go;
}

void Scene::DeleteCurrentScene()
{
	for (GameObject* gameObject : gameObjectList)
	{
		RELEASE(gameObject);
	}
	engine->GetPhysics()->ResetCollisionBodyToObjectMap();
	gameObjectList.clear();
	gameObjectList.shrink_to_fit();
	lights.clear();
	lights.shrink_to_fit();
	engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.clear();
	engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.shrink_to_fit();
	rootGo = new GameObject(0, engine, "Root");
	gameObjectList.push_back(rootGo);
}

void Scene::DeleteGameObject(GameObject* gameObject)
{
	if (gameObject->GetParent() == nullptr) return;

	for (int i = gameObject->children.size()-1;i>=0;--i)
	{
		DeleteGameObject(gameObject->children.at(i));
	}

	for (int i = 0; i < engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size(); i++)
	{
		if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[i] == gameObject->GetUID())
			engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.erase(engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.begin() + i);
	}
	
	auto position = std::find(gameObjectList.begin(), gameObjectList.end(), gameObject);

	gameObjectList.erase(position);
	if (gameObject != nullptr)
	{
		//Re organize children into the top parent so that they are not hanging, plus it deletes the parent's children
		GameObject* parent = gameObject->GetParent();
		std::vector<GameObject*> children = gameObject->GetChildren();
		if (children.size() > 0)
		{
			for (std::vector<GameObject*>::iterator ch = children.begin(); ch != children.end(); ch++)
			{
				GameObject* child = (*ch);
				parent->AttachChild(child);
			}
		}
		
		parent->RemoveChild(gameObject);

		if (gameObject->GetComponent<C_LightSource>() != nullptr)
		{
			RemoveLight(gameObject);
		}
		if (gameObject->GetComponent<C_RigidBody>() != nullptr)
		{
			engine->GetPhysics()->DeleteBodyFromObjectMap(gameObject);
		}
		RELEASE(gameObject);
	}

	engine->GetRenderer()->ResetFrustumCulling();
	engine->GetCamera3D()->currentCamera->ApplyCullings();
}

void Scene::RemoveGameObjectIterator(std::vector<GameObject*>::iterator go)
{
	GameObject* gameObject = (*go);
	if (gameObject != nullptr)
	{
		GameObject* parent = gameObject->GetParent();
		std::vector<GameObject*> children = gameObject->GetChildren();
		if (children.size() > 0)
		{
			for (std::vector<GameObject*>::iterator ch = children.begin(); ch != children.end(); ch++)
			{
				GameObject* child = (*ch);
				GameObject* childParent = child->GetParent();
				childParent = gameObject->GetParent();
				parent->AttachChild(child);
			}
		}
		parent->RemoveChild(gameObject);

		gameObjectList.erase(go);
		gameObject->CleanUp();
		RELEASE(gameObject);
	}
}

void Scene::ComputeQuadTree()
{

		//if (!sceneTreeIsDirty) return;

		//std::vector<GameObject*>* objects = new std::vector<GameObject*>();

		//if (sceneTree != nullptr) delete sceneTree;
		//sceneTree = new QuadTree3D(AABB(float3(-100, -100, -100), float3(100, 100, 100)));

		//ApplyToObjects([objects](GameObject* it) mutable {
		//	objects->push_back(it);
		//	});

		//sceneTree->AddObjects(*objects);
		//delete objects;
	
}

void Scene::AddLight(GameObject* newLight)
{
	if (newLight != nullptr)
		lights.push_back(newLight);
}

void Scene::RemoveLight(GameObject* lightToDelete)
{
	for (std::vector<GameObject*>::iterator light = lights.begin(); light != lights.end();)
	{
		if (lightToDelete == *light)
		{
			light = lights.erase(light);
		}
		else {
			++light;
		}
	}
}

std::vector<GameObject*> Scene::GetLights(SourceType type)
{
	std::vector<GameObject*> ret;

	for (int i = 0; i < lights.size(); i++)
	{
		C_LightSource* light = lights[i]->GetComponent<C_LightSource>();
		if (light == nullptr)
		{
			RemoveLight(lights[i]);
			continue;
		}
		else if(light->GetSourceType() == type)
		{
			ret.push_back(lights[i]);
		}
	}

	return ret;
}

void Scene::SetShadowCaster(GameObject* shadowCaster)
{
	//if there is a shadow caster active maybe do smthng

	this->shadowCaster = shadowCaster;
}

GameObject* Scene::GetShadowCaster()
{
	return shadowCaster;
}

template<class UnaryFunction>
inline void Scene::ApplyToObjects(UnaryFunction f)
{
	recursive_iterate(rootGo, f);
}

template<class UnaryFunction>
inline void Scene::recursive_iterate(const GameObject* o, UnaryFunction f)
{
	for (auto c = o->children.begin(); c != o->children.end(); ++c)
	{
		recursive_iterate(*c, f);
		f(*c);
	}
}