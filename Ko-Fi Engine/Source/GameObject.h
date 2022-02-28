#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Mesh.h"
#include <vector>

#include "Component.h"

class KoFiEngine;
class ComponentTransform;
class ComponentMesh;
class ComponentInfo;

class GameObject
{
public:
	GameObject(uint uid, KoFiEngine* engine, const char* name = nullptr);
	//GameObject(const char* path, int id, const char* name = nullptr);
	~GameObject();

	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate(float dt);
	bool CleanUp();

	void Enable();
	void Disable();

	template<class T> T* CreateComponent()
	{
		T* newComponent = new T(this);
		return newComponent;
	}

	template<class T> T* GetComponent()
	{
		T* component = nullptr;
		for (Component* c : components)
		{
			component = dynamic_cast<T*>(c);
			if (component)
				break;
		}
		return component;
	}

	// New way
	void DeleteComponent(Component* component);
	void AddComponent(Component* component);

	void AttachChild(GameObject* child);
	void RemoveChild(GameObject* child);

	void PropagateTransform();

	// Old way
	void SetName(const char* name);
	const char* GetName();

	std::vector<GameObject*> GetChildren() const;
	void SetChild(GameObject* child);
	GameObject* GetParent() const;

	ComponentTransform* GetTransform();
	std::vector<Component*> GetComponents() const;

	void SetUID(uint uid);
	uint GetUID() const;

	void SetParentUID(uint uid);
	uint GetParentUID() const;

	bool HasChildrenWithUID(uint uid);

	KoFiEngine* GetEngine();

public:
	std::string name;
	bool active = true;

private:
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent = nullptr;
	uint uid;
	uint parentUid;

	KoFiEngine* engine = nullptr;
	ComponentTransform* transform = nullptr;
};

#endif // !__GAMEOBJECT_H__