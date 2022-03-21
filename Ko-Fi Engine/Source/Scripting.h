#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__

#include "Globals.h"
#include "Engine.h"
#include "Input.h" 
#include "SceneManager.h"
#include "SceneIntro.h"

#include "Log.h"
#include "ImGuiAppLog.h"
#include "MathGeoLib/Math/float3.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentScript.h"
#include "ComponentText.h"
#include "ComponentAnimator.h"
#include "ComponentParticle.h"
#include "C_AudioSource.h"
#include "C_AudioSwitch.h"

enum INSPECTOR_VARIABLE_TYPE
{
	INSPECTOR_NO_TYPE,
	INSPECTOR_INT,
	INSPECTOR_FLOAT,
	INSPECTOR_FLOAT2,
	INSPECTOR_FLOAT3,
	INSPECTOR_BOOL,
	INSPECTOR_STRING,
	INSPECTOR_TO_STRING
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
	std::variant<int, float, float2, float3, bool, std::string> value;

	InspectorVariable(std::string name, INSPECTOR_VARIABLE_TYPE type, std::variant<int, float, float2, float3, bool, std::string> value) : name(name), type(type), value(value) {}
};

class Item
{ // Needs porper structure !!
public:
	ItemType type = ITEM_NO_TYPE;
	int damage;

	Item(ItemType type, int damage) : type(type), damage(damage) {}
};

class Scripting
{
public:

	Scripting()
	{
		lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::debug, sol::lib::string);
	}

	~Scripting() {}

	void SetUpVariableTypes()
	{
			/// Lua data structures and functions
			/// Enums:
		// KEY_STATE
		lua.new_enum("KEY_STATE",
			"KEY_IDLE",		KEY_STATE::KEY_IDLE,
			"KEY_DOWN",		KEY_STATE::KEY_DOWN,
			"KEY_REPEAT",	KEY_STATE::KEY_REPEAT,
			"KEY_UP",		KEY_STATE::KEY_UP
		);

		// ComponentType
		lua.new_enum("ComponentType",
			"NONE",			ComponentType::NONE,
			"MESH",			ComponentType::MESH,
			"MATERIAL",		ComponentType::MATERIAL,
			"CAMERA",		ComponentType::CAMERA,
			"COLLIDER",		ComponentType::COLLIDER,
			"SCRIPT",		ComponentType::SCRIPT,
			"TRANSFORM",	ComponentType::TRANSFORM,
			"INFO",			ComponentType::INFO,
			"TRANSFORM2D",	ComponentType::TRANSFORM2D,
			"CANVAS",		ComponentType::CANVAS,
			"IMAGE",		ComponentType::IMAGE,
			"BUTTON",		ComponentType::BUTTON,
			"TEXT",			ComponentType::TEXT
		);

		// INSPECTOR_VARIABLE_TYPE
		lua.new_enum("INSPECTOR_VARIABLE_TYPE",
			"INSPECTOR_NO_TYPE",	INSPECTOR_VARIABLE_TYPE::INSPECTOR_NO_TYPE,
			"INSPECTOR_INT",		INSPECTOR_VARIABLE_TYPE::INSPECTOR_INT,
			"INSPECTOR_FLOAT",		INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT,
			"INSPECTOR_FLOAT2",		INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT2,
			"INSPECTOR_FLOAT3",		INSPECTOR_VARIABLE_TYPE::INSPECTOR_FLOAT3,
			"INSPECTOR_BOOL",		INSPECTOR_VARIABLE_TYPE::INSPECTOR_BOOL
		);

		// ItemType
		lua.new_enum("ItemType",
			"ITEM_NO_TYPE", ItemType::ITEM_NO_TYPE,
			"ITEM_HAND",	ItemType::ITEM_HAND,
			"ITEM_KNIFE",	ItemType::ITEM_KNIFE,
			"ITEM_GUN",		ItemType::ITEM_GUN
			);

		// RuntimeState
		lua.new_enum("RuntimeState",
			"PAUSED",	GameState::PAUSED,
			"PLAYING", GameState::PLAYING,
			"STOPPED", GameState::STOPPED,
			"TICK", GameState::TICK
		);


			/// Classes:
		// float3 structure
		lua.new_usertype<float3>("float3",
			sol::constructors<void(), void(float, float, float)>(),
			"x", &float3::x,
			"y", &float3::y,
			"z", &float3::z
			);

		// float2 structure
		lua.new_usertype<float2>("float2",
			sol::constructors<void(), void(float, float)>(),
			"x", &float2::x,
			"y", &float2::y
			);

		
		// GameObject structure
		lua.new_usertype<GameObject>("GameObject",
			sol::constructors<void()>(),
			"active",				&GameObject::active,
			"name",					&GameObject::name,
			"GetParent",			&GameObject::GetParent,
			"GetComponents",		&GameObject::GetComponents,							// Kinda works... not very useful tho
			"GetTransform",			&GameObject::GetTransform,
			"GetRigidBody",			&GameObject::GetComponent<ComponentRigidBody>,
			"GetText",				&GameObject::GetComponent<ComponentText>,
			"GetComponentAnimator", &GameObject::GetComponent<ComponentAnimator>,
			"GetComponentParticle", &GameObject::GetComponent<ComponentParticle>,
			"GetAudioSwitch",		&GameObject::GetComponent<C_AudioSwitch>,
			"IsSelected",			&GameObject::IsSelected
			/*,"GetComponent", &GameObject::GetComponent<Component>*/				// Further documentation needed to get this as a dynamic cast
			);

		// Component structure
		lua.new_usertype<Component>("Component",
			sol::constructors<void(GameObject*)>(),
			"active",	&Component::active,
			"owner",	&Component::owner,
			"type",		&Component::type,
			"GetType",	&Component::GetType
			);

		// Transform structure
		lua.new_usertype<ComponentTransform>("ComponentTransform",
			sol::constructors<void(GameObject*)>(),
			"GetPosition", &ComponentTransform::GetPosition,
			"SetPosition", &ComponentTransform::SetPosition,
			"GetRotation", &ComponentTransform::GetRotation,
			"SetRotation", &ComponentTransform::SetRotation,
			"GetScale",	   &ComponentTransform::GetScale,
			"SetScale",	   &ComponentTransform::SetScale,
			"GetFront",	   &ComponentTransform::Front,
			"SetFront",	   &ComponentTransform::SetFront
			);

		// Component Text
		lua.new_usertype<ComponentText>("ComponentText",
			sol::constructors<void(GameObject*)>(),
			"GetTextValue", &ComponentText::GetTextValue,
			"SetTextValue", &ComponentText::SetTextValue
			);

		// Component Animator
		lua.new_usertype<ComponentAnimator>("ComponentAnimator",
			sol::constructors<void(GameObject*)>(),
			"SetSelectedClip",	&ComponentAnimator::SetSelectedClip
			);

		// Component Particle
		lua.new_usertype<ComponentParticle>("ComponentParticle",
			sol::constructors<void(GameObject*)>()

			);

		// Component Audio Switch
		lua.new_usertype<C_AudioSwitch>("C_AudioSwitch",
			sol::constructors<void(GameObject*)>(),
			"PlayTrack", &C_AudioSwitch::PlayTrack,
			"PauseTrack", &C_AudioSwitch::PauseTrack,
			"ResumeTrack", &C_AudioSwitch::ResumeTrack,
			"StopTrack", &C_AudioSwitch::StopTrack
			);

		// Inspector Variables
		lua.new_usertype<InspectorVariable>("InspectorVariable",
			sol::constructors<void(std::string, INSPECTOR_VARIABLE_TYPE, std::variant<int, float, float2, float3, bool, std::string>)>(),
			"name",		&InspectorVariable::name,
			"type",		&InspectorVariable::type,
			"value",	&InspectorVariable::value
			);
	
		// Rigid Body structure
		lua.new_usertype<ComponentRigidBody>("ComponentRigidBody",
			sol::constructors<void(GameObject*)>(),
			"IsStatic",				&ComponentRigidBody::IsStatic,
			"IsKinematic",			&ComponentRigidBody::IsKinematic,
			"SetStatic",			&ComponentRigidBody::SetStatic,
			"SetDynamic",			&ComponentRigidBody::SetDynamic,
			"SetLinearVelocity",	&ComponentRigidBody::SetLinearVelocity,
			"FreezePositionY",		&ComponentRigidBody::FreezePositionY,
			"Set2DVelocity",		&ComponentRigidBody::Set2DVelocity
			);

		// Item
		lua.new_usertype<Item>("Item",
			sol::constructors<void(ItemType, int)>(),
			"type",		&Item::type,
			"damage",	&Item::damage
			);


			/// Variables
		lua["gameObject"] = gameObject;
		lua["componentTransform"] = componentTransform;
		

			/// Functions
		lua.set_function("GetInput",			&Scripting::LuaGetInput, this);
		lua.set_function("CreateBullet",		&Scripting::LuaCreateBullet, this);
		lua.set_function("DeleteGameObject",	&Scripting::DeleteGameObject, this);
		lua.set_function("Find",				&Scripting::LuaFind, this);
		lua.set_function("GetInt",				&Scripting::LuaGetInt, this);
		lua.set_function("NewVariable",			&Scripting::LuaNewVariable, this);
		lua.set_function("GetRuntimeState",		&Scripting::LuaGetRuntimeState, this);
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
			case 4:  { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_SPACE); }
			case 5:	 { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_H); }
			case 6:	 { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_K); }
			case 7:  { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_G); }
			case 8:  { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_X); }
			case 9:  { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_C); }
			case 10: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_R); }

			case 21: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_1); }
			case 22: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_2); }
			case 23: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_3); }
			case 24: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_4); }
		}
	}

	void LuaCreateBullet()
	{
		gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToCreate.push_back(gameObject);
	}

	void DeleteGameObject()
	{
		gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToDelete.push_back(gameObject);
	}
	
	GameObject* LuaFind(std::string name)
	{
		for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
		{
			if (go->name == name)
				return go;
		}
		return nullptr;
	}

	int LuaGetInt(std::string path, std::string variable)
	{
		for (GameObject* go : gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList)
		{
			ComponentScript* script = go->GetComponent<ComponentScript>();
			if (script)
			{
				if (path == script->path.substr(script->path.find_last_of('/') + 1))
				{
					int abc = (int)script->handler->lua[variable.c_str()];
					return abc;
				}
			}
		}

		return -999;
	}

	void LuaNewVariable(InspectorVariable* inspectorVariable)
	{
		ComponentScript* script = gameObject->GetComponent<ComponentScript>();
		script->inspectorVariables.push_back(inspectorVariable);
	}

	GameState LuaGetRuntimeState() const
	{
		return gameObject->GetEngine()->GetSceneManager()->GetGameState();
	}

	void LuaPlayAudio()
	{

	}

public:
	sol::state lua;
	GameObject* gameObject = nullptr;
	ComponentTransform* componentTransform = nullptr;
};

#endif // !__SCRIPTING_H__