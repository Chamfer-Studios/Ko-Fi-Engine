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
#include <lua.hpp>
#include <sol.hpp>


class Scripting
{
public:

	Scripting()
	{
		lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::debug);
	}

	~Scripting() {}

	void SetUpVariableTypes()
	{
		// Giving lua certain data structures and functions

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


		// float3 structure
		lua.new_usertype<float3>("float3",
			sol::constructors<void(), void(float, float, float)>(),
			"x", &float3::x,
			"y", &float3::y,
			"z", &float3::z
			);

		
		// GameObject structure
		lua.new_usertype<GameObject>("GameObject",
			sol::constructors<void()>(),
			"active",		&GameObject::active,
			"name",			&GameObject::name,
			"GetParent",	&GameObject::GetParent,
			"GetComponents",&GameObject::GetComponents,							// Kinda works... not very useful tho
			"GetTransform", &GameObject::GetTransform,
			"GetRigidBody", &GameObject::GetComponent<ComponentRigidBody>/*,
			"GetComponent", &GameObject::GetComponent<Component>*/				// Further documentation needed to get this as a dynamic cast
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


		// Rigid Body structure
		lua.new_usertype<ComponentRigidBody>("ComponentRigidBody",
			sol::constructors<void(GameObject*)>(),
			"IsStatic",				&ComponentRigidBody::IsStatic,
			"IsKinematic",			&ComponentRigidBody::IsKinematic,
			"SetStatic",			&ComponentRigidBody::SetStatic,
			"SetDynamic",			&ComponentRigidBody::SetDynamic,
			"SetLinearVelocity",	&ComponentRigidBody::SetLinearVelocity,
			"FreezePositionY",		&ComponentRigidBody::FreezePositionY
			);
	

		// Giving lua certain variables
		lua["gameObject"] = gameObject;
		lua["componentTransform"] = componentTransform;
		lua.set_function("GetInput", &Scripting::LuaGetInput, this);
		lua.set_function("CreateBullet", &Scripting::LuaCreateBullet, this);
		lua.set_function("DeleteGameObject", &Scripting::DeleteGameObject, this);
		lua.set_function("Find", &Scripting::LuaFind, this);
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
			case 4: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_I); }
			case 5: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_J); }
			case 6: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_K); }
			case 7: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_L); }
			case 8: { return gameObject->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_SPACE); }
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

public:
	sol::state lua;
	GameObject* gameObject = nullptr;
	ComponentTransform* componentTransform = nullptr;
};

#endif // !__SCRIPTING_H__