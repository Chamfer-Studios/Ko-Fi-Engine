#pragma once

#include <string>
#include <vector>
#include <variant>

#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"


class GameObject;
class C_Script;
class C_Transform;

typedef std::variant<std::nullptr_t, void*, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*> inspectorVariantType;

static enum INSPECTOR_VARIABLE_TYPE
{
	INSPECTOR_NO_TYPE,
	INSPECTOR_INT,
	INSPECTOR_FLOAT,
	INSPECTOR_FLOAT2,
	INSPECTOR_FLOAT3,
	INSPECTOR_BOOL,
	INSPECTOR_STRING,
	INSPECTOR_TO_STRING,
	INSPECTOR_TEXTAREA,
	INSPECTOR_FLOAT3_ARRAY,
	INSPECTOR_GAMEOBJECT,
};

class InspectorVariable
{
public:
	std::string name;
	INSPECTOR_VARIABLE_TYPE type = INSPECTOR_VARIABLE_TYPE::INSPECTOR_NO_TYPE;

	inspectorVariantType value;

	InspectorVariable(std::string name, INSPECTOR_VARIABLE_TYPE type, inspectorVariantType value) : name(name), type(type), value(value) {}
};

struct ScriptingEvent
{
	ScriptingEvent(std::string _key, std::vector<inspectorVariantType> _fields) {
		this->key = _key;
		this->fields = _fields;
	}

	std::string key;
	std::vector<inspectorVariantType> fields;
};

class LanguageEnvironment {
public:
	LanguageEnvironment(C_Script* _script);
	~LanguageEnvironment();

	virtual bool ReloadScript() = 0;

	virtual void Init() = 0;
	virtual bool Start() = 0;
	virtual bool EventHandler() = 0;
	virtual bool Update(float dt) = 0;
	virtual bool PostUpdate(float dt) = 0;
	virtual bool CleanUp() = 0;

	template<typename... Args>
	bool SafeFunctionCall(std::string name, Args... args) {};

	virtual inspectorVariantType GetProperty(std::string name) = 0;
	virtual void SetProperty(std::string name, inspectorVariantType value) = 0;

	virtual void OnCollisionEnter(GameObject* go) = 0;
	virtual void OnCollisionRepeat(GameObject* go) = 0;
	virtual void OnCollisionExit(GameObject* go) = 0;
	virtual void OnTriggerEnter(GameObject* go) = 0;
	virtual void OnTriggerStay(GameObject* go) = 0;
	virtual void OnTriggerExit(GameObject* go) = 0;
	virtual void OnRayCastHit() = 0;

	GameObject* gameObject = nullptr;
	C_Script* script = nullptr;
	C_Transform* componentTransform = nullptr;
};