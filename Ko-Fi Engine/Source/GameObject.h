#pragma once
#include "Mesh.h"
#include <vector>
#include "Component.h"

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
	Component* GetComponent(COMPONENT_TYPE type);

private:
	std::vector<Component*> components;
	const char* directory;
	std::string name;
	uint id;
	bool active;
};