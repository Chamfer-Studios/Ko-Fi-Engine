#pragma once

#include "LanguageEnvironment.h"

#include "sol.hpp"

#include "M_Input.h"

#include <map>

#include "M_SceneManager.h"
#include "GameObject.h"

#include "duktape.h"

class GameObject;
class C_Script;
class M_Navigation;
class M_Camera3D;
class M_Physics;

class DuktapeJSLanguageEnvironment final : public LanguageEnvironment {
public:
	DuktapeJSLanguageEnvironment(C_Script* _script);
	~DuktapeJSLanguageEnvironment();

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
	duk_context* ctx = nullptr;

public:
	static duk_ret_t Log(duk_context* ctx);
};
