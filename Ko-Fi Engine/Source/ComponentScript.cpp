#include "ComponentScript.h"
#include "Engine.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "Input.h"

#include "Globals.h"
#include "Log.h"
#include "ImGuiAppLog.h"
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
		if ((int)handler->lua["Update"](dt) == 1)
			printf("A");
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
		if (!scriptLoaded)
		{
			ImGui::InputText("Script Name", &fileName);
		}
		else
		{
			ImGui::Text(fileName.c_str());

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
				script = handler->lua.load_file(fullName);
				script();

				isRunning = true;
			}
			if (ImGui::Button("Stop")) // This will be an event call
			{
				isRunning = false;
			}
			if (ImGui::Button("Fetch") && isRunning) // This will be an event call
			{
				float3 dest = float3(0, 0.2, -2);
				handler->lua["SetDestination"](dest);
			}
		}
	}

	if (owner->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_RETURN) == KEY_STATE::KEY_DOWN)
	{
		LoadScript();
	}

	return ret;
}

bool ComponentScript::LoadScript()
{
	if (scriptLoaded) return false;

	fullName = "../Source/" + fileName + ".lua";
	std::ifstream file(fullName.c_str());
	if (file.good())
	{
		scriptLoaded = true;
	}

	return true;
}