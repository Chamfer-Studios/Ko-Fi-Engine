#pragma once
#include "Module.h"
#include "Globals.h"

#include <lua.hpp>
#include <sol.hpp>

class GameObject;

class Scripting : public Module
{
public:
	Scripting(KoFiEngine* engine);
	~Scripting();

	bool Init();
	bool PreUpdate(float dt);
	bool CleanUp();

public:
	sol::state lua;

private:
	KoFiEngine* engine = nullptr;
};