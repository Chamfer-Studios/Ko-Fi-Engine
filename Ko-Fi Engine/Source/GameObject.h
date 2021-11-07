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
	GameObject(uint id);
	GameObject(const char* path, uint id);
	~GameObject();

	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void Enable();
	void Disable();
	Component* CreateComponent(COMPONENT_TYPE type);
	Component* CreateComponent(COMPONENT_TYPE type, COMPONENT_SUBTYPE subtype);
	void SetName(std::string name);
	std::string GetName();
	std::vector<Component*> GetComponents();
	uint GetId();
	Component* GetComponent(COMPONENT_TYPE type);
	std::string name;
	bool active = true;

private:
	std::vector<Component*> components;
	std::string directory;
	uint id;
};