#pragma once

#include "LanguageEnvironment.h"

#include "M_Input.h"

#include <map>

#include "M_SceneManager.h"
#include "GameObject.h"

#include <libplatform/libplatform.h>
#include <v8.h>

class GameObject;
class C_Script;
class M_Navigation;
class M_Camera3D;
class M_Physics;

class V8JSLanguageEnvironment final : public LanguageEnvironment {
public:
	V8JSLanguageEnvironment(C_Script* _script);
	~V8JSLanguageEnvironment();

public:
	bool ReloadScript() final;

	void Init() final;
	bool Start() final;
	bool EventHandler() final;
	bool Update(float dt) final;
	bool PostUpdate(float dt) final;
	bool CleanUp() final;

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

	static void InitV8Platform();
	static void KillV8Platform();

private:
	v8::Local<v8::Context> context;

public:

};