#pragma once
#include "imgui.h"
#include "json.hpp"
using nlohmann::json;
class PanelChooser;
class GameObject;

enum class ComponentType
{
	NONE,
	MESH,
	MATERIAL,
	CAMERA,
	COLLIDER,
	SCRIPT,
	TRANSFORM,
	INFO
};

class Component
{
public:
	Component(GameObject* parent);

	virtual void Enable() { active = true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate(float dt) { return true; }
	virtual bool CleanUp() { return true; }
	virtual bool InspectorDraw(PanelChooser* chooser) { return true; }
	virtual void Disable() { active = false; }
	ComponentType GetType() { return type; }
	

public:
	bool active = true;
	GameObject* owner;
	ComponentType type;
};