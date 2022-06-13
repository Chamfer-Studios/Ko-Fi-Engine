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
#include <functional>
#include "SkyBox.h"
#include "Quadtree.h"

class GameObject;
class KoFiEngine;
class C_LightSource;
enum class SourceType;

class Scene : public Resource
{
public:
	Scene(KoFiEngine* engine) : active(false), engine(engine)
	{
		drawSkybox = true;
	}

	virtual ~Scene()
	{
		CleanUp();
	}

	void Init()
	{
		if (hasInit) return;
		hasInit = true;
		active = true;
	}

	// Called before render is available
	virtual bool Awake()
	{
		if (hasAwaken) return true;
		hasAwaken = true;
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
	bool CleanUp();

	GameObject* GetGameObject(int uid);
	bool IsGameObjectInScene(std::string name);
	void OnAnyButtonHovered(const std::function<void()>& onAnyButtonHovered, const std::function<void()>& onNoButtonHovered);
	void OnAnyEnemyHovered(const std::function<void()>& onAnyEnemyHovered, const std::function<void()>& onNoEnemyHovered);
	void OnAnySpiceSpotHovered(const std::function<void()>& onAnySpiceSpotHovered, const std::function<void()>& onNoSpiceSpotHovered);

	void SwitchCursor(const std::function<void(std::string)>& onChange, const std::function<void()>& onNoSpiceSpotHovered);
	virtual GameObject* CreateEmptyGameObject(const char* name = nullptr, GameObject* parent = nullptr, bool is3D = true);

	//Cleans current Scene
	virtual void DeleteCurrentScene();

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

	void SetShadowCaster(GameObject* shadowCaster);
	GameObject* GetShadowCaster();

public:
	std::string name = "";
	bool active;

	KoFiEngine* engine = nullptr;
	std::vector<GameObject*> gameObjectList;
	std::map<std::string, std::string> gameObjectListToCreate;
	std::vector<GameObject*> gameObjectListToDelete;
	GameObject* rootGo = nullptr;
	GameObject* currentCamera = nullptr;

	// Models in scene map
	// Pairs a GameObject UID with the model Resource UID and the Asset Path (.fbx)
	std::multimap<UID, std::pair<UID, std::string>> sceneModels;
	
	// Space Partitioning
	bool sceneTreeIsDirty = true;
	bool drawSceneTree = false;
	Quadtree sceneTree;

	std::vector<const char*> tags; // TODO: needs to be implemented!

	LineSegment ray;

	GameObject* shadowCaster = nullptr; //current light that casts shadows
	bool drawSkybox;
	SkyBox skybox;

	std::vector<C_LightSource*> lights;

	bool hasInit = false;
	bool hasAwaken = false;
	bool hasStarted = false;
};

#endif // __SCENE_H__