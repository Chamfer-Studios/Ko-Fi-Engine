#ifndef __COMPONENT_SCRIPT_H__
#define __COMPONENT_SCRIPT_H__

#include "Component.h"
#include <lua.hpp>
#include <sol.hpp>

class GameObject;
class ComponentTransform;
using Json = nlohmann::json;

class Scripting;
class InspectorVariable;

class ComponentScript : public Component 
{
public:
	ComponentScript(GameObject* gameObject);
	~ComponentScript();

	bool Start() override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)
	void Save(Json& json) const override;
	void Load(Json& json) override;
	void ReloadScript();

public:
	sol::load_result script; // Check if it can be private
	Scripting* handler = nullptr;
	std::string path = "";
	std::vector<InspectorVariable*> inspectorVariables;

private:
	int numScript;
	bool isScriptLoaded = false;

};

#endif // __COMPONENT_SCRIPT_H__