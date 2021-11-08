#pragma once
#include "Mesh.h"
#include <vector>
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentInfo.h"

class GameObject
{
public:
	GameObject(int id,const char* name = nullptr);
	GameObject(const char* path, int id,const char* name = nullptr);
	~GameObject();

	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool CleanUp();

	void Enable();
	void Disable();
	Component* CreateComponent(COMPONENT_TYPE type);
	Component* CreateComponent(COMPONENT_TYPE type, COMPONENT_SUBTYPE subtype);
	void SetChild(GameObject* go);
	void RemoveChild(GameObject* go);
	void SetName(std::string name);
	std::vector<GameObject*> GetChildren();
	std::string GetName();
	GameObject* GetParent();
	std::vector<Component*> GetComponents();
	uint GetId();
	Component* GetComponent(COMPONENT_TYPE type);
	ComponentTransform* GetTransform();

public:
	std::string name;
	bool active = true;

private:
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent = nullptr;
	std::string directory;
	int id;
};