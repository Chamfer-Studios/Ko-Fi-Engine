#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "R_Mesh.h"
#include "C_Mesh.h"
#include <vector>

#include <unordered_map>
#include <typeindex>

#include "Component.h"

#include <string.h>

#include <optick.h>

class KoFiEngine;
class C_Transform;
class C_Mesh;
class C_Info;
class C_Script;
class C_Collider2;
class C_Animator;
class C_LightSource;

enum class TAG
{
	TAG_UNTAGGED,
	TAG_PLAYER,
	TAG_ENEMY,
	TAG_FLOOR,
	TAG_DECORATION_FLOOR,
	TAG_PICKUP,
	TAG_CORPSE,
	TAG_DIALOGUE,
	TAG_WALL,
};

class GameObject
{
public:
	GameObject(UID uid, KoFiEngine* engine, const char* name = nullptr, bool is3D = true);
	GameObject();
	// GameObject(const char* path, int id, const char* name = nullptr);
	~GameObject();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	bool OnPlay();
	bool OnSceneSwitch();
	bool OnPause();
	bool OnStop();
	bool OnResume();
	bool OnTick();

	void Enable();
	void Disable();

	template <class T>
	T* GetComponent() const {
		OPTICK_EVENT();

		T* component = nullptr;

		std::type_index id = typeid(T);
		for (Component* c : components)
		{
			if (c != nullptr && c->typeIndex == id) {
				component = dynamic_cast<T*>(c);
				if (component) {
					break;
				}
			}
		}
		return component;
	}
	
	std::vector<C_Script*> GetAllScripts();

	// New way
	void DeleteComponent(Component *component);
	void PushBackComponent(Component *component) { components.push_back(component); }
	Component *AddComponentByType(ComponentType componentType);
	void AttachChild(GameObject *child);
	void RemoveChild(GameObject *child);

	void PropagateTransform();

	// Old way
	void SetName(const char *name, bool numbered = true);
	const char *GetName() const;

	std::vector<GameObject *> GetChildren() const;
	GameObject *GetChildWithName(std::string childName);
	void SetChild(GameObject *child);
	GameObject *GetParent() const;

	C_Transform *GetTransform() const;
	std::vector<Component *> GetComponents() const;
	AABB BoundingAABB();
	UID GetUID() const;
	void SetUID(UID uid);

	UID GetParentUID() const;
	void SetParentUID(UID uid);
	inline bool GetRenderGameObject() const { return renderGameObject; }

	bool HasChildrenWithUID(UID uid);
	bool HasParentWithUID(UID uid);

	KoFiEngine *GetEngine() const;
	void SetEngine(KoFiEngine *engine);

	bool PrefabSaveJson();
	bool PrefabSave(Json &jsonFile);
	bool LoadPrefabJson(const char *path, bool exists);
	bool LoadPrefab(Json &jsonFile);
	bool UpdatePrefab(Json &jsonFile);
	inline void SetRenderGameObject(bool renderGameObject) { this->renderGameObject = renderGameObject; }

	bool IsSelected();
	void LoadSceneFromName(std::string name);
	void SetChangeScene(bool changeSceneLua, std::string sceneNameLua);

	void PropragateIsActive();
	void SetIsActiveToChildren(std::vector<GameObject *> &list, bool value);
	void OnStoped();
	void Active(bool isActive);
	void Quit();

private:
	std::string SetObjectNumberedName(const char *_name);

public:
	template <class T>
	T *CreateComponent()
	{
		T *newComponent = new T(this);
		return newComponent;
	}

public:
	bool active = true;
	bool isActiveWindow = false;

	int numScripts = 0;
	bool is3D = true;
	bool isPrefab = false;
	bool changeScene = false;
	bool isQuitting = false;
	std::string sceneName;
	std::string prefabPath;
	TAG tag;

	std::vector<GameObject *> children;

	bool isCulled = false;

private:
	std::string name;
	std::vector<Component*> components;

	std::vector<Component*> componentsToBeDeleted;

	GameObject* parent = nullptr;
	UID uid;
	UID parentUid;
	bool renderGameObject = true;

	KoFiEngine *engine = nullptr;
	C_Transform *transform = nullptr;
	C_Mesh* mesh = nullptr;
};

#endif // !__GAMEOBJECT_H__
