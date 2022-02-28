#pragma once
#ifndef __COMPONENT_SCRIPT_H__
#define __COMPONENT_SCRIPT_H__

#include "Component.h"

#include "Scripting.h"

class GameObject;
class ComponentTransform;

class ComponentScript : public Component
{
public:
	ComponentScript(GameObject* gameObject);
	~ComponentScript();

	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

public:
	sol::load_result script;
	
private:
	std::string fileName;
	bool isRunning = false; // Temp
	ComponentTransform* componentTransform = nullptr;
};

#endif // __COMPONENT_SCRIPT_H__