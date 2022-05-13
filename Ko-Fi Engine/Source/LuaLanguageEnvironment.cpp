#include "LuaLanguageEnvironment.h"

#include "C_Script.h"
#include "Log.h"
#include "ImGuiAppLog.h"

#include "optick.h"

#include "ApplicationInterface.h"

LuaLanguageEnvironment::LuaLanguageEnvironment(C_Script* _script) : LanguageEnvironment(_script)
{
}

LuaLanguageEnvironment::~LuaLanguageEnvironment()
{
}

bool LuaLanguageEnvironment::ReloadScript()
{
	if (script->s->path == "")
		return true;

	auto scriptFile = lua.script_file(script->s->path);
	if (scriptFile.valid()) {
		// Call succeeded
	}
	else {
		// Call failed
		sol::error err = scriptFile;
		std::string what = err.what();
		appLog->AddLog("%s\n", what.c_str());
	}
	script->s->isScriptLoaded = true;

	return true;
}

void LuaLanguageEnvironment::Init()
{
	lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::coroutine, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::debug, sol::lib::string);

	/// Lua data structures and functions
		/// Enums:
		// KEY_STATE
	lua.new_enum("KEY_STATE",
		"KEY_IDLE", KEY_STATE::KEY_IDLE,
		"KEY_DOWN", KEY_STATE::KEY_DOWN,
		"KEY_REPEAT", KEY_STATE::KEY_REPEAT,
		"KEY_UP", KEY_STATE::KEY_UP);

	// ComponentType
	lua.new_enum("ComponentType",
		"NONE", ComponentType::NONE,
		"MESH", ComponentType::MESH,
		"MATERIAL", ComponentType::MATERIAL,
		"CAMERA", ComponentType::CAMERA,
		"BOX_COLLIDER", ComponentType::BOX_COLLIDER,
		"SPHERE_COLLIDER", ComponentType::SPHERE_COLLIDER,
		"CAPSULE_COLLIDER", ComponentType::CAPSULE_COLLIDER,
		"RIGID_BODY", ComponentType::RIGID_BODY,
		"SCRIPT", ComponentType::SCRIPT,
		"TRANSFORM", ComponentType::TRANSFORM,
		"INFO", ComponentType::INFO,
		"TRANSFORM2D", ComponentType::TRANSFORM2D,
		"CANVAS", ComponentType::CANVAS,
		"IMAGE", ComponentType::IMAGE,
		"BUTTON", ComponentType::BUTTON,
		"TEXT", ComponentType::TEXT);

	// INSPECTOR_VARIABLE_TYPE
	lua.new_enum("INSPECTOR_VARIABLE_TYPE",
		"INSPECTOR_NO_TYPE", INSPECTOR_VARIABLE_TYPE::INSPECTOR_NO_TYPE,
		"INSPECTOR_INT", INSPECTOR_VARIABLE_TYPE::INSPECTOR_INT,
		"INSPECTOR_FLOAT", INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT,
		"INSPECTOR_FLOAT2", INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT2,
		"INSPECTOR_FLOAT3", INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT3,
		"INSPECTOR_BOOL", INSPECTOR_VARIABLE_TYPE::INSPECTOR_BOOL,
		"INSPECTOR_STRING", INSPECTOR_VARIABLE_TYPE::INSPECTOR_STRING,
		"INSPECTOR_TO_STRING", INSPECTOR_VARIABLE_TYPE::INSPECTOR_TO_STRING,
		"INSPECTOR_FLOAT3_ARRAY", INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT3_ARRAY,
		"INSPECTOR_GAMEOBJECT", INSPECTOR_VARIABLE_TYPE::INSPECTOR_GAMEOBJECT
	);

	// RuntimeState
	lua.new_enum("RuntimeState",
		"PAUSED", GameState::PAUSED,
		"PLAYING", GameState::PLAYING,
		"STOPPED", GameState::STOPPED,
		"TICK", GameState::TICK);

	// Tags
	lua.new_enum("Tag",
		"UNTAGGED", Tag::TAG_UNTAGGED,
		"PLAYER", Tag::TAG_PLAYER,
		"ENEMY", Tag::TAG_ENEMY,
		"WALL", Tag::TAG_WALL,
		"PROJECTILE", Tag::TAG_PROJECTILE);

	/// Classes:
	// float3 structure
	lua.new_usertype<float3>("float3",
		sol::constructors<void(), void(float, float, float)>(),
		"x", &float3::x,
		"y", &float3::y,
		"z", &float3::z,
		"Normalize", &float3::Normalize);

	// float2 structure
	lua.new_usertype<float2>("float2",
		sol::constructors<void(), void(float, float)>(),
		"x", &float2::x,
		"y", &float2::y);
	// float4 structure
	lua.new_usertype<float4>("float4",
		sol::constructors<void(), void(float, float, float, float)>(),
		"x", &float4::x,
		"y", &float4::y,
		"z", &float4::z,
		"w", &float4::w
		);

	// Quaternion structure
	lua.new_usertype<Quat>("Quat",
		sol::constructors<void(float, float, float, float)>(),
		"x", &Quat::x,
		"y", &Quat::y,
		"z", &Quat::z,
		"w", &Quat::w,
		"RotateY", &Quat::RotateY
		);
	// GameObject structure
	lua.new_usertype<GameObject>("GameObject",
		sol::constructors<void()>(),
		"active", &GameObject::active,
		"GetName", &GameObject::GetName,
		"GetUID", &GameObject::GetUID,
		"tag", &GameObject::tag,
		"GetParent", &GameObject::GetParent,
		"GetChild", &GameObject::GetChildWithName,
		"GetComponents", &GameObject::GetComponents, // Kinda works... not very useful tho
		"GetTransform", &GameObject::GetTransform,
		"GetC_Mesh", &GameObject::GetComponent<C_Mesh>,
		"GetRigidBody", &GameObject::GetComponent<C_RigidBody>,
		"GetBoxCollider", &GameObject::GetComponent<C_BoxCollider>,
		"GetText", &GameObject::GetComponent<C_Text>,
		"GetComponentAnimator", &GameObject::GetComponent<C_Animator>,
		"GetComponentParticle", &GameObject::GetComponent<C_Particle>,
		"GetAudioSwitch", &GameObject::GetComponent<C_AudioSwitch>,
		"GetCamera", &GameObject::GetComponent<C_Camera>,
		"IsSelected", &GameObject::IsSelected,
		"GetButton", &GameObject::GetComponent<C_Button>,
		"GetImage", &GameObject::GetComponent<C_Image>,
		"OnStoped", &GameObject::OnStoped,
		"LoadScene", &GameObject::LoadSceneFromName,
		"Active", &GameObject::Active,
		"Quit", &GameObject::Quit,
		"ChangeScene", &GameObject::SetChangeScene
		);


	// Component structure
	lua.new_usertype<Component>("Component",
		sol::constructors<void(GameObject*)>(),
		"active", &Component::active,
		"owner", &Component::owner,
		"type", &Component::type,
		"GetType", &Component::GetType);

	// Transform structure
	lua.new_usertype<C_Transform>("C_Transform",
		sol::constructors<void(GameObject*)>(),
		"GetPosition", &C_Transform::GetPosition,
		"SetPosition", &C_Transform::SetPosition,
		"GetRotation", &C_Transform::GetRotationEuler,
		"SetRotation", &C_Transform::SetRotationEuler,
		"SetRotationQuat", &C_Transform::SetRotationQuat,
		"GetRotationQuat", &C_Transform::GetRotationQuat,
		"GetScale", &C_Transform::GetScale,
		"SetScale", &C_Transform::SetScale,
		"GetFront", &C_Transform::Front,
		"GetGlobalFront", &C_Transform::GlobalFront,
		"GetRight", &C_Transform::Right,
		"GetUp", &C_Transform::Up,
		"LookAt", &C_Transform::LookAt
		);

	// Component Camera
	lua.new_usertype<C_Camera>("C_Transform",
		sol::constructors<void(GameObject*)>(),
		"LookAt", &C_Camera::LookAt,
		"right", &C_Camera::GetRight,
		"up", &C_Camera::GetUp
		);

	// Component Mesh
	lua.new_usertype<C_Mesh>("C_Mesh",
		sol::constructors<void(GameObject*)>(),
		"Disable", &C_Mesh::Disable,
		"Enable", &C_Mesh::Enable
		);

	// Component Text
	lua.new_usertype<C_Text>("C_Text",
		sol::constructors<void(GameObject*)>(),
		"GetTextValue", &C_Text::GetTextValue,
		"SetTextValue", &C_Text::SetTextValue);

	// Component Image
	lua.new_usertype<C_Image>("C_Image",
		sol::constructors<void(GameObject*)>(),
		"SetTexture", &C_Image::SetTexture,
		"GetTexturePath", &C_Image::GetTexturePath
		);

	lua.new_usertype<C_Button>("C_Button",
		sol::constructors<void(GameObject*)>(),
		"IsPressed", &C_Button::IsPressed,
		"IsIdle", &C_Button::IsIdle,
		"IsHovered", &C_Button::IsHovered);

	// Component Animator
	lua.new_usertype<C_Animator>("ComponentAnimator",
		sol::constructors<void(GameObject*)>(),
		"SetSelectedClip", &C_Animator::SetSelectedClip,
		"IsCurrentClipLooping", &C_Animator::IsCurrentClipLooping,
		"IsCurrentClipPlaying", &C_Animator::IsCurrentClipPlaying);

	// Component Particle
	lua.new_usertype<C_Particle>("C_Particle",
		sol::constructors<void(GameObject*)>(),
		"StopParticleSpawn", &C_Particle::StopParticleSpawn,
		"ResumeParticleSpawn", &C_Particle::ResumeParticleSpawn);

	// Component Audio Switch
	lua.new_usertype<C_AudioSwitch>("C_AudioSwitch",
		sol::constructors<void(GameObject*)>(),
		"PlayTrack", &C_AudioSwitch::PlayTrack,
		"PauseTrack", &C_AudioSwitch::PauseTrack,
		"ResumeTrack", &C_AudioSwitch::ResumeTrack,
		"StopTrack", &C_AudioSwitch::StopTrack);

	// Inspector Variables
	lua.new_usertype<InspectorVariable>("InspectorVariable",
		sol::constructors<void(std::string, INSPECTOR_VARIABLE_TYPE, inspectorVariantType)>(),
		"name", &InspectorVariable::name,
		"type", &InspectorVariable::type,
		"value", &InspectorVariable::value);

	// Rigid Body structure
	lua.new_usertype<C_RigidBody>("C_RigidBody",
		sol::constructors<void(GameObject*)>(),
		"IsStatic", &C_RigidBody::IsStatic,
		"IsKinematic", &C_RigidBody::IsKinematic,
		"SetStatic", &C_RigidBody::SetBodyStatic,
		"SetDynamic", &C_RigidBody::SetBodyDynamic,
		"FreezePositionY", &C_RigidBody::FreezePositionY,
		"SetLinearVelocity", &C_RigidBody::SetLinearVelocity,
		"SetRigidBodyPos", &C_RigidBody::SetRigidBodyPos,
		"SetUseGravity", &C_RigidBody::SetUseGravity,
		"UpdateEnableGravity", &C_RigidBody::UpdateEnableGravity);

	lua.new_usertype<C_BoxCollider>("C_BoxCollider",
		sol::constructors<void(GameObject*)>(),
		"IsTrigger", &C_BoxCollider::GetIsTrigger,
		"SetTrigger", &C_BoxCollider::SetIsTrigger,
		"GetFilter", &C_BoxCollider::GetFilter,
		"SetFilter", &C_BoxCollider::SetFilter,
		"UpdateFilter", &C_BoxCollider::UpdateFilter,
		"UpdateIsTrigger", &C_BoxCollider::UpdateIsTrigger);

	lua.new_usertype<M_Navigation>("M_Navigation",
		sol::constructors<void(KoFiEngine*)>(),
		"FindPath", &M_Navigation::FindPath);

	lua.new_usertype<M_Camera3D>("M_Camera3D",
		sol::constructors<void(KoFiEngine*)>(),
		"WorldToScreen", &M_Camera3D::WorldToScreen);

	/// Variables
	lua["gameObject"] = gameObject;
	lua["componentTransform"] = componentTransform;

	/// Functions
	lua.set_function("GetInput", &LuaLanguageEnvironment::LuaGetInput, this);
	lua.set_function("InstantiatePrefab", &LuaLanguageEnvironment::LuaInstantiatePrefab, this);
	lua.set_function("InstantiateNamedPrefab", &LuaLanguageEnvironment::LuaInstantiateNamedPrefab, this);
	lua.set_function("DeleteGameObject", &LuaLanguageEnvironment::DeleteGameObject, this);
	lua.set_function("Find", &LuaLanguageEnvironment::LuaFind, this);
	lua.set_function("GetObjectsByTag", &LuaLanguageEnvironment::LuaGetObjectsByTag, this);
	lua.set_function("GetVariable", &LuaLanguageEnvironment::LuaGetVariable, this);
	lua.set_function("SetVariable", &LuaLanguageEnvironment::LuaSetVariable, this);
	lua.set_function("NewVariable", &LuaLanguageEnvironment::LuaNewVariable, this);
	lua.set_function("GetRuntimeState", &LuaLanguageEnvironment::LuaGetRuntimeState, this);
	lua.set_function("GetGameObjectHovered", &LuaLanguageEnvironment::LuaGetGameObjectHovered, this);
	lua.set_function("GetLastMouseClick", &LuaLanguageEnvironment::LuaGetLastMouseClick, this);
	lua.set_function("Log", &LuaLanguageEnvironment::LuaLog, this);
	lua.set_function("GetCamera", &LuaLanguageEnvironment::GetCamera, this);
	lua.set_function("GetNavigation", &LuaLanguageEnvironment::GetNavigation, this);
	lua.set_function("GetPhysics", &LuaLanguageEnvironment::GetPhysics, this);
	lua.set_function("SetLuaVariableFromGameObject", &LuaLanguageEnvironment::LuaSetLuaVariableFromGameObject, this);
	lua.set_function("MulQuat", &LuaLanguageEnvironment::LuaMulQuat, this);
	lua.set_function("DispatchEvent", &LuaLanguageEnvironment::DispatchEvent, this);
	lua.set_function("DispatchGlobalEvent", &LuaLanguageEnvironment::DispatchGlobalEvent, this);
	lua.set_function("RayCast", &LuaLanguageEnvironment::RayCast, this);
	lua.set_function("StartTimer", &LuaLanguageEnvironment::StartTimer, this);
	lua.set_function("PeekTimer", &LuaLanguageEnvironment::PeekTimer, this);
}

bool LuaLanguageEnvironment::Start()
{
	SafeFunctionCall("Start");

	return true;
}

bool LuaLanguageEnvironment::EventHandler()
{
	while (script->eventQueue.size() != 0) {
		auto e = script->eventQueue.front();
		script->eventQueue.pop();

		SafeFunctionCall("EventHandler", e.key, e.fields);
	}

	return true;
}

bool LuaLanguageEnvironment::Update(float dt)
{
	OPTICK_EVENT();

	SafeFunctionCall("Update", dt);

	return true;
}

bool LuaLanguageEnvironment::PostUpdate(float dt)
{
	SafeFunctionCall("PostUpdate", dt);

	return true;
}

bool LuaLanguageEnvironment::CleanUp()
{
	CONSOLE_LOG("Quitting Lua scripting system");
	appLog->AddLog("Quitting Lua scripting system\n");
	return true;
}

sol::protected_function LuaLanguageEnvironment::GetFunction(std::string name)
{
	OPTICK_EVENT();

	if (functions.find(name) == functions.end()) {
		functions[name] = lua[name];
	}

	sol::protected_function function = functions[name];

	return function;
}

template<typename... Args>
bool LuaLanguageEnvironment::SafeFunctionCall(std::string name, Args... args)
{
	OPTICK_EVENT();

	sol::protected_function function = GetFunction(name);

	if (script->owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PLAYING && script->s->isScriptLoaded)
	{
		if (function.valid()) {
			sol::protected_function_result result = function(args...);
			if (result.valid()) {
				// Call succeeded
			}
			else {
				// Call failed
				sol::error err = result;
				std::string what = err.what();
				appLog->AddLog("%s\n", what.c_str());
			}
		}
	}

	return true;
}

inspectorVariantType LuaLanguageEnvironment::GetProperty(std::string name)
{
	auto variable = lua[name];

	if (variable.valid()) return variable;
	return nullptr;
}

void LuaLanguageEnvironment::SetProperty(std::string name, inspectorVariantType value)
{
	lua[name] = value;
}

void LuaLanguageEnvironment::OnCollisionEnter(GameObject* go)
{
	SafeFunctionCall("OnCollisionEnter", go);
}

void LuaLanguageEnvironment::OnCollisionRepeat(GameObject* go)
{
	SafeFunctionCall("OnCollisionRepeat", go);
}

void LuaLanguageEnvironment::OnCollisionExit(GameObject* go)
{
	SafeFunctionCall("OnCollisionExit", go);
}

void LuaLanguageEnvironment::OnTriggerEnter(GameObject* go)
{
	SafeFunctionCall("OnTriggerEnter", go);
}

void LuaLanguageEnvironment::OnTriggerStay(GameObject* go)
{
	SafeFunctionCall("OnTriggerStay", go);
}

void LuaLanguageEnvironment::OnTriggerExit(GameObject* go)
{
	SafeFunctionCall("OnTriggerExit", go);
}

void LuaLanguageEnvironment::OnRayCastHit()
{
	SafeFunctionCall("OnRayCastHit", nullptr);
}

void LuaLanguageEnvironment::StartTimer()
{
	PERF_START(script->ptimer);
}

void LuaLanguageEnvironment::PeekTimer(std::string note)
{
	appLog->AddLog("%s ... %f ms\n", note.c_str(), script->ptimer.ReadMs());
}

inline KEY_STATE LuaLanguageEnvironment::LuaGetInput(int button)
{
	if (button < 4 && button > 0)
		return gameObject->GetEngine()->GetInput()->GetMouseButton(button);

	switch (button)
	{
	case 4:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_SPACE);
	}
	case 5:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_H);
	}
	case 6:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_K);
	}
	case 7:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_G);
	}
	case 8:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_X);
	}
	case 9:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_C);
	}
	case 10:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_R);
	}
	case 11:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_B);
	}
	case 12:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_D);
	}
	case 13:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_T);
	}
	case 14:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_Q);
	}
	case 15:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_E);
	}

	case 21:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_1);
	}
	case 22:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_2);
	}
	case 23:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_3);
	}
	case 24:
	{
		return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_4);
	}
	}
}

inline void LuaLanguageEnvironment::RayCast(float3 startPoint, float3 endPoint, std::string filterName, GameObject* senderGo)
{
	return gameObject->GetEngine()->GetPhysics()->RayCastHits(startPoint, endPoint, filterName, senderGo);
}

inline M_Navigation* LuaLanguageEnvironment::GetNavigation()
{
	return gameObject->GetEngine()->GetNavigation();
}

inline M_Camera3D* LuaLanguageEnvironment::GetCamera()
{
	return gameObject->GetEngine()->GetCamera3D();
}

inline M_Physics* LuaLanguageEnvironment::GetPhysics()
{
	return gameObject->GetEngine()->GetPhysics();
}

inline void LuaLanguageEnvironment::LuaInstantiateNamedPrefab(std::string prefab, std::string name)
{
	gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToCreate.emplace(name, prefab);
}

inline void LuaLanguageEnvironment::LuaInstantiatePrefab(std::string name)
{
	gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToCreate.emplace(name, name);
}

inline void LuaLanguageEnvironment::DeleteGameObject()
{
	gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToDelete.push_back(gameObject);
}

inline GameObject* LuaLanguageEnvironment::LuaFind(std::string name)
{
	OPTICK_EVENT();

	for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
	{
		if (go->GetName() == name)
			return go;
	}
	return nullptr;
}

inline std::vector<GameObject*> LuaLanguageEnvironment::LuaGetObjectsByTag(Tag tag)
{
	std::vector<GameObject*> ret;
	for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
	{
		if (go->tag == tag)
		{
			ret.push_back(go);
		}
	}
	return ret;
}

inline inspectorVariantType LuaLanguageEnvironment::LuaGetVariable(std::string path, std::string variable, INSPECTOR_VARIABLE_TYPE type)
{
	for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
	{
		C_Script* script = go->GetComponent<C_Script>();
		if (script)
		{

			if (path == script->s->path.substr(script->s->path.find_last_of('/') + 1))
			{
				return script->s->handler->GetProperty(variable.c_str());
			}
		}
	}

	return -999.0f;
}

inline void LuaLanguageEnvironment::LuaSetVariable(inspectorVariantType value, std::string path, std::string variable, INSPECTOR_VARIABLE_TYPE type)
{
	for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
	{
		C_Script* script = go->GetComponent<C_Script>();
		if (script)
		{
			if (path == script->s->path.substr(script->s->path.find_last_of('/') + 1))
			{
				script->s->handler->SetProperty(variable.c_str(), value);
			}
		}
	}
}

inline void LuaLanguageEnvironment::LuaNewVariable(InspectorVariable* inspectorVariable)
{
	for (std::vector<InspectorVariable*>::iterator var = script->s->inspectorVariables.begin(); var != script->s->inspectorVariables.end(); ++var)
	{
		if (inspectorVariable->name == (*var)->name)
		{
			switch (inspectorVariable->type)
			{
			case INSPECTOR_INT:
			{
				lua[inspectorVariable->name.c_str()] = std::get<float>((*var)->value);
				return;
			}
			case INSPECTOR_FLOAT:
			{
				lua[inspectorVariable->name.c_str()] = std::get<float>((*var)->value);
				return;
			}
			case INSPECTOR_FLOAT2:
			{
				lua[inspectorVariable->name.c_str()] = std::get<float2>((*var)->value);
				return;
			}
			case INSPECTOR_FLOAT3:
			{
				lua[inspectorVariable->name.c_str()] = std::get<float3>((*var)->value);
				return;
			}
			case INSPECTOR_BOOL:
			{
				lua[inspectorVariable->name.c_str()] = std::get<bool>((*var)->value);
				return;
			}
			case INSPECTOR_STRING:
			{
				lua[inspectorVariable->name.c_str()] = std::get<std::string>((*var)->value);
				return;
			}
			case INSPECTOR_TO_STRING:
			{
				lua[inspectorVariable->name.c_str()] = std::get<std::string>((*var)->value);
				return;
			}
			case INSPECTOR_FLOAT3_ARRAY:
			{
				lua[inspectorVariable->name.c_str()] = std::get<std::vector<float3>>((*var)->value);
				return;
			}
			case INSPECTOR_GAMEOBJECT:
			{
				lua[inspectorVariable->name.c_str()] = std::get<GameObject*>((*var)->value);
				return;
			}
			}
		}
	}
	script->s->inspectorVariables.push_back(inspectorVariable);
}

inline GameState LuaLanguageEnvironment::LuaGetRuntimeState() const
{
	return gameObject->GetEngine()->GetSceneManager()->GetGameState();
}

inline GameObject* LuaLanguageEnvironment::LuaGetGameObjectHovered()
{
	return gameObject->GetEngine()->GetCamera3D()->MousePicking();
}

inline float3 LuaLanguageEnvironment::LuaGetLastMouseClick()
{
	return gameObject->GetEngine()->GetCamera3D()->GetLastMouseClick();
}

inline void LuaLanguageEnvironment::LuaLog(const char* log)
{
	appLog->AddLog(log);
}

inline void LuaLanguageEnvironment::LuaSetLuaVariableFromGameObject(GameObject* go, std::string variable, inspectorVariantType value)
{
	if (go == nullptr)
		return;


	C_Script* goScript = go->GetComponent<C_Script>();
	if (goScript == nullptr)
		return;


	goScript->s->handler->SetProperty(variable.c_str(), value);

}

inline void LuaLanguageEnvironment::DrawCone(float3 position, float3 forward, float3 up, float angle, int length) {
	gameObject->GetEngine()->GetRenderer()->DrawCone(position, forward, up, angle, length);
}

inline float3 LuaLanguageEnvironment::LuaMulQuat(Quat quat, float3 vector)
{
	float3 tmp = quat.Mul(vector);
	return tmp;
}

inline void LuaLanguageEnvironment::DispatchGlobalEvent(std::string key, std::vector<inspectorVariantType> fields) {
	for (auto go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList) {
		for (auto c : go->GetComponents()) {
			if (c->type == ComponentType::SCRIPT) {
				((C_Script*)c)->eventQueue.push(ScriptingEvent(key, fields));
			}
		}
	}
}

inline void LuaLanguageEnvironment::DispatchEvent(std::string key, std::vector<inspectorVariantType> fields) {
	OPTICK_EVENT();

	for (auto c : gameObject->GetComponents()) {
		if (c->type == ComponentType::SCRIPT) {
			((C_Script*)c)->eventQueue.push(ScriptingEvent(key, fields));
		}
	}
}

inline void LuaLanguageEnvironment::DrawCircle(float range, float3 position)
{
	gameObject->GetEngine()->GetRenderer()->DrawCircle(position, range);
}
