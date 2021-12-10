#pragma once
#include "imgui.h"

class PanelChooser;
class GameObject;

enum class ComponentType
{
	TRANSFORM,
	MESH,
	MATERIAL,
	INFO,
	CAMERA
};

class Component
{
public:
	Component(GameObject* parent);
	virtual ~Component() {}

	virtual void Enable() { active = true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate() { return true; }
	virtual bool Update() { return true; }
	virtual bool PostUpdate() { return true; }
	virtual bool CleanUp() { return true; }
	virtual bool InspectorDraw(PanelChooser* chooser) { return true; }
	virtual void Disable() { active = false; }
	ComponentType GetType() { return type; }

protected:
	bool active = true;
	GameObject* owner;
	ComponentType type;
};