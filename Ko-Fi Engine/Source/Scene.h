#ifndef __SCENE_H__
#define __SCENE_H__

#include "GameObject.h"
#include "Importer.h"
#include "Engine.h"
#include "RNG.h"

#include <vector>
#include "MathGeoLib/Geometry/LineSegment.h"

class Scene
{
public:
	Scene() : active(false)
	{
	}

	~Scene()
	{
		CleanUp();
	}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake()
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate(float dt)
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	GameObject* GetGameObject(int uid)
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

	virtual GameObject* CreateEmptyGameObject(const char* name = nullptr, GameObject* parent=nullptr,bool is3D = true)
	{
		GameObject* go = new GameObject(gameObjectList.size(), engine, name, is3D);
		this->gameObjectList.push_back(go);
		if (parent)
			parent->AttachChild(go);
		else
			this->rootGo->AttachChild(go);

		return go;
	}

	virtual void DeleteGameObject(GameObject* gameObject)
	{
		for (std::vector<GameObject*>::iterator it = gameObjectList.begin(); it != gameObjectList.end(); ++it)
		{
			if ((*it)->GetUID() == gameObject->GetUID())
			{
				gameObjectList.erase(it);
				break;
			}
		}
		
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

			gameObject->CleanUp();
			

			RELEASE(gameObject);
		}

	}

	void RemoveGameObjectIterator(std::vector<GameObject*>::iterator go)
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

public:
	std::string name;
	bool active;

	KoFiEngine* engine = nullptr;
	std::vector<GameObject*> gameObjectList;
	std::vector<GameObject*> gameObjectListToCreate;
	GameObject* rootGo = nullptr;
	GameObject* currentCamera = nullptr;

	LineSegment ray;
};

#endif // !__SCENE_H__