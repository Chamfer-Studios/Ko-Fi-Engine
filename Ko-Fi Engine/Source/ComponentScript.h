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
	bool Update() override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

private:
	sol::load_result script1;
	std::string fileName;

	ComponentTransform* componentTransform = nullptr;
};

#endif // __COMPONENT_SCRIPT_H__