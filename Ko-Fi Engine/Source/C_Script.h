#pragma once

#include "Component.h"
#include <lua.hpp>
#include <sol.hpp>

#include <queue>

#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"

class Scripting;
class GameObject;
class C_Transform;
class vector;
class InspectorVariable;
class C_Script;

using Json = nlohmann::json;

struct ScriptHandler
{
	ScriptHandler(GameObject* owner, C_Script* script);

	sol::protected_function_result script; // Check if it can be private
	Scripting* handler = nullptr;
	std::string path = "";
	std::vector<InspectorVariable*> inspectorVariables;
	sol::protected_function lua_update;
	bool isScriptLoaded = false;
};

struct ScriptingEvent
{
	ScriptingEvent(std::string _key, std::vector<std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>>> _fields) {
		this->key = _key;
		this->fields = _fields;
	}

	std::string key;
	std::vector<std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>>> fields;
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

	std::queue<ScriptingEvent> eventQueue;
};