#include "ComponentScript.h"
#include "Engine.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "Input.h"

#include "Globals.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "PanelChooser.h"
#include "imgui_stdlib.h"
#include <fstream>
#include "MathGeoLib/Math/float2.h"

ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = ComponentType::SCRIPT;

	owner->numScripts++;
	numScript = owner->numScripts;

	handler = new Scripting();

	handler->gameObject = owner;
	handler->componentTransform = owner->GetTransform();
	handler->SetUpVariableTypes();
}

ComponentScript::~ComponentScript()
{
	delete(handler);
	handler = nullptr;
}

bool ComponentScript::Start()
{
	bool ret = true;


	return ret;
}

bool ComponentScript::CleanUp()
{
	handler->CleanUp();
	return true;
}

bool ComponentScript::Update(float dt)
{
	if (isRunning && active)
	{
		handler->lua["Update"](dt);
	}
	return true;
}

bool ComponentScript::PostUpdate(float dt)
{

	return true;
}

bool ComponentScript::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	std::string headerName = "Script" + std::to_string(numScript);

	if (ImGui::CollapsingHeader(headerName.c_str()))
	{

		if (chooser->IsReadyToClose("LoadScript")) 
		{
			if (chooser->OnChooserClosed() != nullptr) 
			{
				path = chooser->OnChooserClosed();
				script = handler->lua.load_file(path);
			}
		}
		if (ImGui::Button("Select Script")) 
		{
			chooser->OpenPanel("LoadScript", "lua");
		}
		ImGui::SameLine();
		ImGui::Text(path.substr(path.find_last_of('/') + 1).c_str());


		if (isRunning)
		{
			float s = handler->lua["speed"];
			if (ImGui::DragFloat("Speed", &s))
			{
				handler->lua["speed"] = s;
			}
		}

		if (ImGui::Button("Run")) // This will be an event call
		{
			script(); // TODO: This doens't save changes done to the .lua file. We need to call load_file again!!!
			isRunning = true;
		}
		if (isRunning)
		{
			if (ImGui::Button("Stop")) // This will be an event call
			{
				isRunning = false;
			}
		}
	}


	return ret;
}

bool ComponentScript::LoadScript(std::string path)
{

	return true;
}

void ComponentScript::SetRunning(const bool& setTo)
{
	if (setTo != isRunning)
		isRunning = setTo;
}

void ComponentScript::Save(Json& json) const
{
	json["type"] = "script";
	json["file_name"] = path;
	json["script_number"] = numScript;
}

void ComponentScript::Load(Json& json)
{
	path = json.at("file_name");
	numScript = json.at("script_number");
}