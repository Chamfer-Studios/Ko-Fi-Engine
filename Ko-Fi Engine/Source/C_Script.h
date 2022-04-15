#ifndef __C_SCRIPT_H__
#define __C_SCRIPT_H__

#include "Component.h"
#include <lua.hpp>
#include <sol.hpp>

class Scripting;
class GameObject;
class C_Transform;
class vector;
class InspectorVariable;

using Json = nlohmann::json;

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

class C_Script : public Component 
{
public:
	C_Script(GameObject* gameObject);
	~C_Script();

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

	void SetId(int id);

	int nScripts = 0;
	ScriptHandler* s;
	int id = -1;
};

#endif // __C_SCRIPT_H__