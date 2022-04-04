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
	NONE = 0,
	MESH ,
	MATERIAL,
	PARTICLE,
	CAMERA,
	COLLIDER,
	SCRIPT,
	RIGID_BODY,
	COLLIDER2,
	AUDIO_SOURCE,
	AUDIO_SWITCH,
	ANIMATOR,
	CANVAS,
	IMAGE,
	BUTTON,
	TEXT,
	WALKABLE,
	FOLLOW_PATH,
	LIGHT_SOURCE,
	END, //Put all the component types that you dont want the add combo to show after the END and map it in the component type to string function below this enum
	TRANSFORM,
	TRANSFORM2D,
	INFO
};
namespace componentTypeUtils{
	const char* ComponentTypeToString(ComponentType e);
	
}

class Component
{
public:
	Component(GameObject *parent);
	virtual ~Component(){};

	virtual bool Start() { return true; }
	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate(float dt) { return true; }
	virtual bool CleanUp() { return true; }

	virtual void Enable() { active = true; }
	virtual void Disable() { active = false; }

	virtual bool OnPlay() { return true; }
	virtual bool OnPause() { return true; }
	virtual bool OnStop() { return true; }
	virtual bool OnResume() { return true; }
	virtual bool OnTick() { return true; }

	virtual bool InspectorDraw(PanelChooser *chooser) { return true; }

	virtual void Save(Json &json) const {}
	virtual void Load(Json &json) {}

	ComponentType GetType() { return type; }

protected:
	void DrawDeleteButton(GameObject *owner, Component *component);
	const char* GetNameByComponentType(ComponentType type);

public:
	bool active = true;
	GameObject *owner = nullptr;
	ComponentType type;
};

#endif //__COMPONENT_H__