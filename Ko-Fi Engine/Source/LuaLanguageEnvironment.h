#pragma once

#include "LanguageEnvironment.h"

#include "sol.hpp"

#include "M_Input.h"

#include <map>

#include "M_SceneManager.h"
#include "GameObject.h"

class GameObject;
class C_Script;
class M_Navigation;
class M_Camera3D;
class M_Physics;

class LuaLanguageEnvironment final : public LanguageEnvironment {
public:
	LuaLanguageEnvironment(C_Script* _script);
	~LuaLanguageEnvironment();

public:
	bool ReloadScript() final;

	void Init() final;
	bool Start() final;
	bool EventHandler() final;
	bool Update(float dt) final;
	bool PostUpdate(float dt) final;
	bool CleanUp() final;

	sol::protected_function GetFunction(std::string name);

	template<typename... Args>
	bool SafeFunctionCall(std::string name, Args... args);

	inspectorVariantType GetProperty(std::string name) final;
	void SetProperty(std::string name, inspectorVariantType value) final;

	void OnCollisionEnter(GameObject* go);
	void OnCollisionRepeat(GameObject* go);
	void OnCollisionExit(GameObject* go);
	void OnTriggerEnter(GameObject* go);
	void OnTriggerStay(GameObject* go);
	void OnTriggerExit(GameObject* go);
	void OnRayCastHit();

private:
	sol::state lua;
	std::map<std::string, sol::protected_function> functions;

public:
	void StartTimer();
	void PeekTimer(std::string note);

	KEY_STATE LuaGetInput(int button);

	void RayCast(float3 startPoint, float3 endPoint, std::string filterName, GameObject* senderGo);

	M_Navigation* GetNavigation();

	M_Camera3D* GetCamera();

	M_Physics* GetPhysics();

	void LuaInstantiateNamedPrefab(std::string prefab, std::string name);

	void LuaInstantiatePrefab(std::string name);

	void DeleteGameObject();

	GameObject* LuaFind(std::string name);

	std::vector<GameObject*> LuaGetObjectsByTag(Tag tag);

	inspectorVariantType LuaGetVariable(std::string path, std::string variable, INSPECTOR_VARIABLE_TYPE type);

	void LuaSetVariable(inspectorVariantType value, std::string path, std::string variable, INSPECTOR_VARIABLE_TYPE type);

	void LuaNewVariable(InspectorVariable* inspectorVariable);

	GameState LuaGetRuntimeState() const;

	GameObject* LuaGetGameObjectHovered();

	float3 LuaGetLastMouseClick();

	void LuaLog(const char* log);

	void LuaSetLuaVariableFromGameObject(GameObject* go, std::string variable, inspectorVariantType value);

	void DrawCone(float3 position, float3 forward, float3 up, float angle, int length);

	float3 LuaMulQuat(Quat quat, float3 vector);

	void DispatchGlobalEvent(std::string key, std::vector<inspectorVariantType> fields);

	void DispatchEvent(std::string key, std::vector<inspectorVariantType> fields);

	void DrawCircle(float range, float3 position);
};