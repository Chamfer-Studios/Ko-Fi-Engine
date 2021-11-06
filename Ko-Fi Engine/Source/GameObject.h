#pragma once
#include "Mesh.h"
#include <vector>
#include "Component.h"

enum PrimitiveTypes;

class GameObject
{
public:
	GameObject(PrimitiveTypes primitiveType, uint id);
	GameObject(const char* path, uint id);
	~GameObject();
	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	void Enable();
	void Disable();
	Component* CreateComponent(COMPONENT_TYPE type);
	void SetName(std::string name);
	std::string GetName();
	std::vector<Component*> GetComponents();

private:
	std::vector<Component*> components;
	const char* directory;
	std::string name;
	uint id;
	bool active;
};