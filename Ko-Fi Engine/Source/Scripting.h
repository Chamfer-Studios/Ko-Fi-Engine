#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__

#include "Globals.h"
#include "Engine.h"
#include "M_Navigation.h"
#include "M_Renderer3D.h"
#include "M_Input.h" 
#include "M_SceneManager.h"
#include "M_Physics.h"
#include "SceneIntro.h"
#include "M_Camera3D.h"
#include "ImGuiAppLog.h"

#include <vector>

#include "Log.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/Quat.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Script.h"
#include "C_Text.h"
#include "C_Image.h"
#include "C_Button.h"
#include "C_Animator.h"
#include "C_Particle.h"
#include "C_Camera.h"
#include "C_AudioSource.h"
#include "C_AudioSwitch.h"
#include "C_Script.h"
#include "C_RigidBody.h"
#include "C_BoxCollider.h"
#include "C_LightSource.h"

enum INSPECTOR_VARIABLE_TYPE
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

enum ItemType
{
	ITEM_NO_TYPE,
	ITEM_HAND,
	ITEM_KNIFE,
	ITEM_GUN
};

class InspectorVariable
{
public:
	std::string name;
	INSPECTOR_VARIABLE_TYPE type = INSPECTOR_NO_TYPE;

	std::variant<int, unsigned int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*> value;

	InspectorVariable(std::string name, INSPECTOR_VARIABLE_TYPE type, std::variant<int, unsigned int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*> value) : name(name), type(type), value(value) {}
};

class Scripting
{
public:
	Scripting(C_Script* _script)
	{
		script = _script;
		lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::coroutine, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::debug, sol::lib::string);
	}

	~Scripting() {}

	void SetUpVariableTypes()
	{
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
									 "SetName", &GameObject::SetName,
									 "GetUID", &GameObject::GetUID,
									 "tag", &GameObject::tag,
									 "GetParent", &GameObject::GetParent,
									 "GetChild", &GameObject::GetChildWithName,
									 "GetComponents", &GameObject::GetComponents, // Kinda works... not very useful tho
									 "GetTransform", &GameObject::GetTransform,
									 "GetLight", &GameObject::GetComponent<C_LightSource>,
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

			/*,"GetComponent", &GameObject::GetComponent<Component>*/ // Further documentation needed to get this as a dynamic cast
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
			sol::constructors<void(GameObject *)>(),
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
			sol::constructors<void(std::string, INSPECTOR_VARIABLE_TYPE, std::variant<int, unsigned int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*>)>(),
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

		lua.new_usertype<C_LightSource>("C_LightSource",
			sol::constructors<void(GameObject*)>(),
			"SetDirection", &C_LightSource::SetDirection,
			"SetAngle", &C_LightSource::SetAngle,
			"SetRange", &C_LightSource::SetRange);

		lua.new_usertype<M_Navigation>("M_Navigation",
			sol::constructors<void(KoFiEngine*)>(),
			"FindPath", &M_Navigation::FindPath);

		lua.new_usertype<M_Camera3D>("M_Camera3D",
			sol::constructors<void(KoFiEngine*)>(),
			"WorldToScreen", &M_Camera3D::WorldToScreen);

		/*lua.new_usertype<M_Physics>("M_Physics",
			sol::constructors<void(KoFiEngine*)>(),
			"Raycast", &M_Physics::Raycast);*/

			/// Variables
		lua["gameObject"] = gameObject;
		lua["componentTransform"] = componentTransform;

		/// Functions
		lua.set_function("GetInput", &Scripting::LuaGetInput, this);
		lua.set_function("InstantiatePrefab", &Scripting::LuaInstantiatePrefab, this);
		lua.set_function("InstantiateNamedPrefab", &Scripting::LuaInstantiateNamedPrefab, this);
		lua.set_function("DeleteGameObject", &Scripting::DeleteGameObject, this);
		lua.set_function("Find", &Scripting::LuaFind, this);
		lua.set_function("GetObjectsByTag", &Scripting::LuaGetObjectsByTag, this);
		lua.set_function("GetVariable", &Scripting::LuaGetVariable, this);
		lua.set_function("SetVariable", &Scripting::LuaSetVariable, this);
		lua.set_function("NewVariable", &Scripting::LuaNewVariable, this);
		lua.set_function("GetRuntimeState", &Scripting::LuaGetRuntimeState, this);
		lua.set_function("GetGameObjectHovered", &Scripting::LuaGetGameObjectHovered, this);
		lua.set_function("GetLastMouseClick", &Scripting::LuaGetLastMouseClick, this);
		lua.set_function("Log", &Scripting::LuaLog, this);
		lua.set_function("GetCamera", &Scripting::GetCamera, this);
		lua.set_function("GetNavigation", &Scripting::GetNavigation, this);
		lua.set_function("GetPhysics", &Scripting::GetPhysics, this);
		lua.set_function("SetLuaVariableFromGameObject", &Scripting::LuaSetLuaVariableFromGameObject, this);
		lua.set_function("MulQuat", &Scripting::LuaMulQuat, this);
		lua.set_function("DispatchEvent", &Scripting::DispatchEvent, this);
		lua.set_function("DispatchGlobalEvent", &Scripting::DispatchGlobalEvent, this);
		lua.set_function("RayCast", &Scripting::RayCast, this);
		lua.set_function("DrawCone", &Scripting::DrawCone, this);
		lua.set_function("DrawLine", &Scripting::DrawLine, this);
	}

	bool CleanUp()
	{
		CONSOLE_LOG("Quitting scripting system");
		appLog->AddLog("Quitting scripting system\n");
		return true;
	}

	KEY_STATE LuaGetInput(int button)
	{
		if (button < 4 && button > 0)
			return gameObject->GetEngine()->GetInput()->GetMouseButton(button);

		switch (button)
		{
			case 4:
			{
				return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_SPACE);
			}
			/*case 5:
			{
				return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_H);
			}*/
			case 6:
			{
				return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_Z);
			}
			/*case 7:
			{
				return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_G);
			}*/
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
			/*case 11:
			{
				return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_B);
			}*/
			case 12:
			{
				return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_LSHIFT);
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

	void RayCast(float3 startPoint, float3 endPoint, std::string filterName, GameObject* senderGo, sol::function callback)
	{
		return gameObject->GetEngine()->GetPhysics()->RayCastHits(startPoint, endPoint, filterName, senderGo, &callback);
	}

	M_Navigation* GetNavigation()
	{
		return gameObject->GetEngine()->GetNavigation();
	}

	M_Camera3D* GetCamera()
	{
		return gameObject->GetEngine()->GetCamera3D();
	}

	M_Physics* GetPhysics()
	{
		return gameObject->GetEngine()->GetPhysics();
	}

	void LuaInstantiateNamedPrefab(std::string prefab, std::string name)
	{
		gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToCreate.emplace(name, prefab);
	}

	void LuaInstantiatePrefab(std::string name)
	{
		gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToCreate.emplace(name, name);
	}

	void DeleteGameObject()
	{
		gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToDelete.push_back(gameObject);
	}

	GameObject* LuaFind(std::string name)
	{
		for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
		{
			if (go->GetName() == name)
				return go;
		}
		return nullptr;
	}

	std::vector<GameObject*> LuaGetObjectsByTag(Tag tag)
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

	std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*> LuaGetVariable(std::string path, std::string variable, INSPECTOR_VARIABLE_TYPE type)
	{
		for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
		{
			C_Script* script = go->GetComponent<C_Script>();
			if (script)
			{

				if (path == script->s->path.substr(script->s->path.find_last_of('/') + 1))
				{
					switch (type)
					{
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_INT:
					{
						return (int)script->s->handler->lua[variable.c_str()];
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT:
					{
						return (float)script->s->handler->lua[variable.c_str()];
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT2:
					{
						return (float2)script->s->handler->lua[variable.c_str()];
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT3:
					{
						return (float3)script->s->handler->lua[variable.c_str()];
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_BOOL:
					{
						return (bool)script->s->handler->lua[variable.c_str()];
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_STRING:
					{
						std::string a = script->s->handler->lua[variable.c_str()];
						return a;
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_GAMEOBJECT:
					{
						return (GameObject*)script->s->handler->lua[variable.c_str()];
					}
					}
				}
			}
		}

		return -999;
	}

	void LuaSetVariable(std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*> value, std::string path, std::string variable, INSPECTOR_VARIABLE_TYPE type)
	{
		for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
		{
			C_Script* script = go->GetComponent<C_Script>();
			if (script)
			{
				if (path == script->s->path.substr(script->s->path.find_last_of('/') + 1))
				{
					switch (type)
					{
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_INT:
					{
						script->s->handler->lua[variable.c_str()] = std::get<int>(value);
						return;
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT:
					{
						script->s->handler->lua[variable.c_str()] = std::get<float>(value);
						return;
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT2:
					{
						script->s->handler->lua[variable.c_str()] = std::get<float2>(value);
						return;
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT3:
					{
						script->s->handler->lua[variable.c_str()] = std::get<float3>(value);
						return;
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_BOOL:
					{
						script->s->handler->lua[variable.c_str()] = std::get<bool>(value);
						return;
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_STRING:
					{
						script->s->handler->lua[variable.c_str()] = std::get<std::string>(value);
						return;
					}
					case INSPECTOR_VARIABLE_TYPE::INSPECTOR_GAMEOBJECT:
					{
						script->s->handler->lua[variable.c_str()] = std::get<GameObject*>(value);
						return;
					}
					}
				}
			}
		}
	}

	void LuaNewVariable(InspectorVariable* inspectorVariable)
	{
		for (std::vector<InspectorVariable*>::iterator var = script->s->inspectorVariables.begin(); var != script->s->inspectorVariables.end(); ++var)
		{
			if (inspectorVariable->name == (*var)->name)
			{
				switch (inspectorVariable->type)
				{
				case INSPECTOR_INT:
				{
					lua[inspectorVariable->name.c_str()] = std::get<int>((*var)->value);
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

	GameState LuaGetRuntimeState() const
	{
		return gameObject->GetEngine()->GetSceneManager()->GetGameState();
	}

	GameObject* LuaGetGameObjectHovered()
	{
		return gameObject->GetEngine()->GetCamera3D()->MousePicking();
	}

	float3 LuaGetLastMouseClick()
	{
		return gameObject->GetEngine()->GetCamera3D()->GetLastMouseClick();
	}

	void LuaLog(const char* log)
	{
		appLog->AddLog(log);
	}

	void LuaSetLuaVariableFromGameObject(GameObject* go, std::string variable, std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*> value)
	{
		if (go == nullptr)
			return;


		C_Script* goScript = go->GetComponent<C_Script>();
		if (goScript == nullptr)
			return;


		goScript->s->handler->lua[variable.c_str()] = value;

	}

	void DrawCone(float3 position, float3 forward, float3 up, float angle, int length) {
		gameObject->GetEngine()->GetRenderer()->DrawCone(position, forward, up, angle, length);
	}

	void DrawLine(float3 a, float3 b) {
		gameObject->GetEngine()->GetRenderer()->DrawLine(a, b);
	} 

	float3 LuaMulQuat(Quat quat, float3 vector)
	{
		float3 tmp = quat.Mul(vector);
		return tmp;
	}

	void DispatchGlobalEvent(std::string key, std::vector<std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*>> fields) {
		for (auto go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList) {
			for (auto c : go->GetComponents()) {
				if (c->type == ComponentType::SCRIPT) {
					((C_Script*)c)->eventQueue.push(ScriptingEvent(key, fields));
				}
			}
		}
	}

	void DispatchEvent(std::string key, std::vector<std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject*>> fields) {
		for (auto c : gameObject->GetComponents()) {
			if (c->type == ComponentType::SCRIPT) {
				((C_Script*)c)->eventQueue.push(ScriptingEvent(key, fields));
			}
		}
	}

	void DrawCircle(float range, float3 position)
	{
		gameObject->GetEngine()->GetRenderer()->DrawCircle(position, range);
	}

public:
	sol::state lua;
	GameObject* gameObject = nullptr;
	C_Transform* componentTransform = nullptr;
	C_Script* script = nullptr;
};

#endif // !__SCRIPTING_H__