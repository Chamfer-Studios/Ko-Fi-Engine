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
#include <lua.hpp>
#include <sol.hpp>


class Scripting
{
public:

	Scripting()
	{
		lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, /*sol::lib::string,*/ sol::lib::debug);
	}

	~Scripting() {}

	void SetUpVariableTypes()
	{
		// Giving lua certain data structures and functions

		// KEY_STATE
		lua.new_enum("KEY_STATE",
			"KEY_IDLE",	  KEY_STATE::KEY_IDLE,
			"KEY_DOWN",   KEY_STATE::KEY_DOWN,
			"KEY_REPEAT", KEY_STATE::KEY_REPEAT,
			"KEY_UP",     KEY_STATE::KEY_UP
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
			"GetTransform", &GameObject::GetTransform
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


		// Giving lua certain variables
		lua["gameObject"] = gameObject;
		lua["componentTransform"] = componentTransform;
		lua.set_function("GetMouseButton", &Scripting::LuaGetMouseButton, this);
		lua.set_function("CreateBullet", &Scripting::LuaCreateBullet, this);
		lua.set_function("DeleteGameObject", &Scripting::DeleteGameObject, this);
	}

	bool CleanUp()
	{
		CONSOLE_LOG("Quitting scripting system");
		appLog->AddLog("Quitting scripting system\n");
		return true;
	}

	KEY_STATE LuaGetMouseButton(int button)
	{
		if (button < 5 && button > 0)
			return gameObject->GetEngine()->GetInput()->GetMouseButton(button);
	}

	void LuaCreateBullet()
	{
		gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToCreate.push_back(gameObject);
	}

	void DeleteGameObject()
	{
		gameObject->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectListToDelete.push_back(gameObject);
	}
	

public:
	sol::state lua;
	GameObject* gameObject = nullptr;
	ComponentTransform* componentTransform = nullptr;
};

#endif // !__SCRIPTING_H__