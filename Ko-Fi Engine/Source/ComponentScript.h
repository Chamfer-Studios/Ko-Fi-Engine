#pragma once

#include "Component.h"
#include <lua.hpp>
#include <sol.hpp>
#include <vector>

class Scripting;

class GameObject;
class ComponentTransform;
using Json = nlohmann::json;

class InspectorVariable;

struct ScriptHandler
{
	ScriptHandler(GameObject* owner);

	sol::protected_function_result script; // Check if it can be private
	Scripting* handler = nullptr;
	std::string path = "";
	std::vector<InspectorVariable*> inspectorVariables;
	sol::protected_function lua_update;
	bool isScriptLoaded = false;
};

class ComponentScript : public Component 
{
public:
	ComponentScript(GameObject* gameObject);
	~ComponentScript();

	bool Start() override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool OnPlay() override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)
	void Save(Json& json) const override;
	void Load(Json& json) override;
	void LoadInspectorVariables(Json& json);
	void ReloadScript(ScriptHandler* script);

	int nScripts = 0;
	std::vector<ScriptHandler*> scripts;
};