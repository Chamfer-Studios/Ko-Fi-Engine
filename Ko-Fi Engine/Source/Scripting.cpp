#include "Globals.h"
#include "Scripting.h"

#include "Engine.h"

#include "Log.h"
#include "ImGuiAppLog.h"



Scripting::Scripting(KoFiEngine* engine) : Module()
{
	name = "Scripting";
	lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math);

	this->engine = engine;
}

// Destructor
Scripting::~Scripting()
{

}

// Called before render is available
bool Scripting::Init()
{
	bool ret = true;
	
	return ret;
}

// Called every draw update
bool Scripting::PreUpdate(float dt)
{
	
	return true;
}

// Called before quitting
bool Scripting::CleanUp()
{
	CONSOLE_LOG("Quitting scripting system");
	appLog->AddLog("Quitting scripting system\n");
	return true;
}