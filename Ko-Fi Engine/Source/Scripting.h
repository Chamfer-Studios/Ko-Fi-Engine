#pragma once
#include "Globals.h"

#include "Log.h"
#include "ImGuiAppLog.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include <lua.hpp>
#include <sol.hpp>

class Scripting
{
public:

	Scripting()
	{
		lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math);
	}

	~Scripting() {}

	void SetUpVariableTypes()
	{
		// Here we give lua certain data structures and variables

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
			"active", &GameObject::active
			);

		// Transform structure
		lua.new_usertype<ComponentTransform>("ComponentTransform",
			sol::constructors<void(GameObject*)>(),
			"GetPosition", &ComponentTransform::GetPosition,
			"SetPosition", &ComponentTransform::SetPosition
			/*,
			"y", &ComponentTransform::GetPosition.y,
			"z", ComponentTransform->GetPosition().z*/
			);

		// Transform
		lua["componentTransform"] = componentTransform;
	}

	bool CleanUp()
	{
		CONSOLE_LOG("Quitting scripting system");
		appLog->AddLog("Quitting scripting system\n");
		return true;
	}

public:
	sol::state lua;
	ComponentTransform* componentTransform = nullptr;
};