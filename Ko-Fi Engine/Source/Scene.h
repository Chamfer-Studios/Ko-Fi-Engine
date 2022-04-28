#ifndef __SCENE_H__
#define __SCENE_H__

#include "Importer.h"
#include "RNG.h"
#include "Resource.h"

#include "MathGeoLib/Geometry/LineSegment.h"

#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

class GameObject;
class KoFiEngine;
class C_LightSource;
class QuadTree3D;
enum class SourceType;

class Scene : public Resource
{
public:
	Scene() : active(false)
	{}

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

	GameObject* GetGameObject(int uid);

	bool IsGameObjectInScene(std::string name);

	virtual GameObject* CreateEmptyGameObject(const char* name = nullptr, GameObject* parent = nullptr, bool is3D = true);

	virtual void DeleteCurrentScene(std::string rootName);

	virtual void DeleteGameObject(GameObject* gameObject);

	void RemoveGameObjectIterator(std::vector<GameObject*>::iterator go);

	template<class UnaryFunction>
	void ApplyToObjects(UnaryFunction f);

	template<class UnaryFunction>
	void recursive_iterate(const GameObject* o, UnaryFunction f);

	void ComputeQuadTree(); // Compute Space Partitioning

	// ----- Lighting -----
	void AddLight(GameObject* newLight);

	void RemoveLight(GameObject* lightToDelete);

	std::vector<GameObject*> GetLights(SourceType type);


public:
	std::string name = "";
	bool active;

	KoFiEngine* engine = nullptr;
	std::vector<GameObject*> gameObjectList;
	std::map<GameObject*, std::string> gameObjectListToCreate;
	std::vector<GameObject*> gameObjectListToDelete;
	GameObject* rootGo = nullptr;
	GameObject* currentCamera = nullptr;
	
	//Space Partitioning
	bool sceneTreeIsDirty = true;
	bool drawSceneTree = false;
	QuadTree3D* sceneTree = nullptr;

	std::vector<const char*> tags; // TODO: needs to be implemented!

	std::vector<GameObject*> lights;
	LineSegment ray;

};


#endif // __SCENE_H__
