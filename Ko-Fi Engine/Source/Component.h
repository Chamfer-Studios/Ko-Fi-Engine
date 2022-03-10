#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "imgui.h"
#include "json.hpp"

using nlohmann::json;
class PanelChooser;
class GameObject;
using Json = nlohmann::json;

enum class ComponentType
{
	NONE,
	MESH,
	MATERIAL,
	PARTICLE,
	CAMERA,
	COLLIDER,
	SCRIPT,
	RIGID_BODY,
	TRANSFORM2D,
	CANVAS,
	IMAGE,
	BUTTON,
	TEXT,
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

	virtual void Save(Json& json) const {}
	virtual void Load(Json& json) {}

	ComponentType GetType() { return type; }


public:
	bool active = true;
	GameObject* owner;
	ComponentType type;
};

#endif // !__COMPONENT_H__
