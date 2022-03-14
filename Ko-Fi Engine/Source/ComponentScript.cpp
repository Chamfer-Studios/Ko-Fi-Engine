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

	ReloadScript();  // For loading scenes

	return ret;
}

bool ComponentScript::CleanUp()
{
	handler->CleanUp();
	return true;
}

bool ComponentScript::Update(float dt)
{
	if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PLAYING && isScriptLoaded)
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

		for (InspectorVariable* variable : inspectorVariables)
		{
			if (variable->type == INSPECTOR_NO_TYPE)
				continue;
			
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
					ImGui::DragFloat(variable->name.c_str(), &std::get<float>(variable->value));
					break;
				}
			}
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
	script = handler->lua.load_file(path);
	script();
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