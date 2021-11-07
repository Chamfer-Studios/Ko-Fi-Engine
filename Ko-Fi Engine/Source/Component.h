#pragma once
#include "imgui.h"

class GameObject;
class PanelChooser;
enum class COMPONENT_TYPE
{
	COMPONENT_TRANSFORM = 0,
	COMPONENT_MESH,
	COMPONENT_MATERIAL,
	COMPONENT_INFO,
};

enum class COMPONENT_SUBTYPE
{
	COMPONENT_MESH_MESH = 0,
	COMPONENT_MESH_CUBE,
	COMPONENT_MESH_SPHERE,
	COMPONENT_MESH_CYLINDER,
	COMPONENT_MESH_LINE,
	COMPONENT_MESH_PLANE,
	COMPONENT_MESH_PYRAMID,
};

class Component {
public:

	Component(COMPONENT_TYPE type): type(type),active(true) {}
	~Component(){}

	virtual void Enable() { active = true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate() { return true; }
	virtual bool Update() {return true;}
	virtual bool PostUpdate() {return true;}
	virtual bool InspectorDraw(PanelChooser* chooser) {return true;}
	virtual void Disable() { active = false; }
public:
	COMPONENT_TYPE type;
protected:
	bool active;
	GameObject* owner;

};