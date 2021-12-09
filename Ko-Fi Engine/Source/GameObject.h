#pragma once
#include "Mesh.h"
#include <vector>

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentInfo.h"

class GameObject
{
public:
	GameObject(int id, const char* name = nullptr);
	GameObject(const char* path, int id, const char* name = nullptr);
	~GameObject();

	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
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

	//New way
	void DeleteComponent(Component* component);
	void AddComponent(Component* component);
	void AttachChild(GameObject* child);
	void RemoveChild(GameObject* child);
	void PropagateTransform();
	//Old way
	void SetName(std::string name);
	std::vector<GameObject*> GetChildren();
	void SetChild(GameObject*child);
	std::string GetName();
	GameObject* GetParent();
	ComponentTransform* GetTransform();
	std::vector<Component*> GetComponents();
	void SetId(int id);
	uint GetId();
	bool HasChildrenWithId(int id);

public:
	std::string name;
	bool active = true;

private:
	ComponentTransform* transform = nullptr;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent = nullptr;
	int id;
};