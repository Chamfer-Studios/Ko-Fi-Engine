#pragma once

#include "Component.h"
#include <lua.hpp>
#include <sol.hpp>

#include <queue>

#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"

#include "PerfTimer.h"

#include "LuaLanguageEnvironment.h"

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

	LanguageEnvironment* handler = nullptr;
	std::string path = "";
	std::vector<InspectorVariable*> inspectorVariables;
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
	bool OnSceneSwitch() override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)
	void Save(Json& json) const override;
	void Load(Json& json) override;
	void LoadInspectorVariables(Json& json);
	void RemoveOldVariables();
	void ReloadScript();

	void SetId(int id);

	ScriptHandler* s = nullptr;
	int id = -1;

	std::queue<ScriptingEvent> eventQueue;

	PerfTimer ptimer;
};