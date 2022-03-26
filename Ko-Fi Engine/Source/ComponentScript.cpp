#include "Scripting.h"
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
#include <vector>
#include "MathGeoLib/Math/float3.h"
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
	handler->CleanUp();
	//RELEASE(handler);
	inspectorVariables.clear();
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
	if (owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PLAYING && isScriptLoaded)
	{
		sol::protected_function_result result = lua_update(dt);
		if (result.valid()) {
			// Call succeeded
			appLog->AddLog("Ok\n");
		}
		else {
			// Call failed
			sol::error err = result;
			std::string what = err.what();
			appLog->AddLog("%s\n", what.c_str());
		}
	}
	return true;
}

bool ComponentScript::PostUpdate(float dt)
{

	return true;
}

bool ComponentScript::OnPlay()
{
	bool ret = true;

	ReloadScript();

	return ret;
}

bool ComponentScript::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	std::string headerName = "Script" + std::to_string(numScript);

	if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (chooser->IsReadyToClose("LoadScript")) 
		{
			if (chooser->OnChooserClosed() != nullptr) 
			{
				path = chooser->OnChooserClosed();
				ReloadScript();
			}
		}
		if (ImGui::Button("Select Script")) 
		{
			chooser->OpenPanel("LoadScript", "lua");
		}
		ImGui::SameLine();
		ImGui::Text(path.substr(path.find_last_of('/') + 1).c_str());

		bool isSeparatorNeeded = true;
		for (InspectorVariable* variable : inspectorVariables)
		{
			if (variable->type == INSPECTOR_NO_TYPE)
				continue;

			if (isSeparatorNeeded)
			{
				ImGui::Separator();
				isSeparatorNeeded = false;
			}
			
			switch (variable->type)
			{
				case INSPECTOR_INT:
				{
					if (ImGui::DragInt(variable->name.c_str(), &std::get<int>(variable->value)))
					{
						handler->lua[variable->name.c_str()] = std::get<int>(variable->value);
					}
					break;
				}
				case INSPECTOR_FLOAT:
				{
					if (ImGui::DragFloat(variable->name.c_str(), &std::get<float>(variable->value)))
					{
						handler->lua[variable->name.c_str()] = std::get<float>(variable->value);
					}
					break;
				}
				case INSPECTOR_FLOAT2:
				{
					if (ImGui::DragFloat2(variable->name.c_str(), std::get<float2>(variable->value).ptr()))
					{
						handler->lua[variable->name.c_str()] = std::get<float2>(variable->value);
					}
					break;
				}
				case INSPECTOR_FLOAT3:
				{
					if (ImGui::DragFloat3(variable->name.c_str(), std::get<float3>(variable->value).ptr()))
					{
						handler->lua[variable->name.c_str()] = std::get<float3>(variable->value);
					}
					break;
				}
				case INSPECTOR_BOOL:
				{
					if (ImGui::Checkbox(variable->name.c_str(), &std::get<bool>(variable->value)))
					{
						handler->lua[variable->name.c_str()] = std::get<bool>(variable->value);
					}
					break;
				}
				case INSPECTOR_STRING:
				{
					if (ImGui::InputText(variable->name.c_str(), &std::get<std::string>(variable->value)))
					{
						handler->lua[variable->name.c_str()] = std::get<std::string>(variable->value);
					}
					break;
				}
				case INSPECTOR_TO_STRING:
				{
					ImGui::Text(std::get<std::string>(variable->value).c_str());
					break;
				}
				case INSPECTOR_FLOAT3_ARRAY:
				{
					int nWaypoints = std::get<std::vector<float3>>(variable->value).size();
					std::vector<float3> waypoints = std::get<std::vector<float3>>(variable->value);
					if (ImGui::DragInt("Path length", &nWaypoints, 1.0f, 0)) {
						waypoints.clear();
						for (int i = 0; i < nWaypoints; i++) {
							waypoints.push_back(float3(0, 0, 0));
						}
						std::get<std::vector<float3>>(variable->value) = waypoints;
					}

					ImGui::Text("Waypoints: ");
					for (int i = 0; i < nWaypoints; i++) {
						std::string label = std::to_string(i);
						if (ImGui::DragFloat3(label.c_str(), &(waypoints[i][0]), 0.5f)) {
							handler->lua[variable->name.c_str()] = std::get<std::vector<float3>>(variable->value);
						}
					}
				}
			}
		}

		if (!isSeparatorNeeded)
		{
			ImGui::Separator();
		}

		if (ImGui::Button("Reload Script"))
		{
			ReloadScript();
		}
	}


	return ret;
}

void ComponentScript::ReloadScript()
{
	if (path == "")
		return;
	inspectorVariables.clear();
	script = handler->lua.script_file(path);
	lua_update = sol::protected_function(handler->lua["Update"]);
	isScriptLoaded = true;
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