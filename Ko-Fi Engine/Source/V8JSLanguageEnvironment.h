#pragma once

#include "LanguageEnvironment.h"

#include "M_Input.h"

#include <map>

#include "M_SceneManager.h"
#include "GameObject.h"

#include "v8.h"
#include "libplatform/libplatform.h"

class GameObject;
class C_Script;
class M_Navigation;
class M_Camera3D;
class M_Physics;

class V8JSLanguageEnvironment final : public LanguageEnvironment {
	struct any {
		enum type { Float, String, VariantVector };
		any(float e) { m_data.FLOAT = e; m_type = Float; }
		any(char* e) { m_data.STRING = e; m_type = String; }
		any(std::vector<inspectorVariantType>* e) { m_data.VARIANT_VECTOR = e; m_type = VariantVector; }
		type get_type() const { return m_type; }
		float get_float() const { return m_data.FLOAT; }
		char* get_string() const { return m_data.STRING; }
		std::vector<inspectorVariantType>* get_variant_vector() const { return m_data.VARIANT_VECTOR; }
	private:
		type m_type;
		union {
			float FLOAT;
			char* STRING;
			std::vector<inspectorVariantType>* VARIANT_VECTOR;
		} m_data;
	};

public:
	V8JSLanguageEnvironment(C_Script* _script);
	~V8JSLanguageEnvironment();

public:
	v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name);
	bool ReloadScript() final;

	void Init() final;
	bool Start() final;
	bool EventHandler() final;
	bool Update(float dt) final;
	bool PostUpdate(float dt) final;
	bool CleanUp() final;

	template<typename... Args>
	bool SafeFunctionCall(std::string name, std::vector<inspectorVariantType> args);

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
	v8::Global<v8::Context> context;

public:

};