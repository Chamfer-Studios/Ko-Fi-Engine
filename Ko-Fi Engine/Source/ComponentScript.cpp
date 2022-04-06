#include "Scripting.h"
#include "ComponentScript.h"
#include "Engine.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "Input.h"
#include "PanelInspector.h"

#include "Globals.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "PanelChooser.h"
#include "imgui_stdlib.h"
#include <fstream>
#include <vector>
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float2.h"
#include "PanelHierarchy.h"

ComponentScript::ComponentScript(GameObject *parent) : Component(parent)
{
	type = ComponentType::SCRIPT;
}

ComponentScript::~ComponentScript()
{
	for (auto s : scripts) {
		s->handler->CleanUp();
		s->inspectorVariables.clear();
	}

	scripts.clear();
}

bool ComponentScript::Start()
{
	bool ret = true;
	return ret;
}

bool ComponentScript::CleanUp()
{
	for (auto s : scripts) {
		s->handler->CleanUp();
		s->inspectorVariables.clear();
	}

	scripts.clear();

	return true;
}

bool ComponentScript::Update(float dt)
{
	for (auto s : scripts) {
		s->lua_update = sol::protected_function(s->handler->lua["Update"]);
		if (owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PLAYING && s->isScriptLoaded)
		{
			if (s->lua_update.valid()) {
				sol::protected_function_result result = s->lua_update(dt);
				if (result.valid()) {
					// Call succeeded
				}
				else {
					// Call failed
					sol::error err = result;
					std::string what = err.what();
					appLog->AddLog("%s\n", what.c_str());
				}
				/*if (owner->changeScene)
				{
					owner->changeScene = false;
					owner->LoadSceneFromName("HUD_Scene");
				}*/
			}
		}
	}
	return true;
}

bool ComponentScript::PostUpdate(float dt)
{
	for (auto s : scripts) {
		if (owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PLAYING && s->isScriptLoaded)
		{
			auto f = s->handler->lua["PostUpdate"];

			if (f.valid()) {
				f(dt);
			}
		}
	}
	return true;
}

bool ComponentScript::OnPlay()
{
	bool ret = true;

	//ReloadScript();

	return ret;
}

bool ComponentScript::InspectorDraw(PanelChooser *chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	std::string headerName = "Script";

	if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		int n = nScripts;
		if (ImGui::DragInt("Num Scripts", &n, 0.1f, 0)) {
			if (n < 0) n = 0;
			if (n < nScripts) {
				scripts.resize(scripts.size() - (nScripts - n));
			}
			else if (n > nScripts) {
				for (int i = nScripts; i < n; i++) {
					scripts.push_back(new ScriptHandler(owner));
				}
			}
			nScripts = n;
		}

		for (auto s : scripts) {
			ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);
			ImGui::Separator();
			if (chooser->IsReadyToClose("LoadScript"))
			{
				if (chooser->OnChooserClosed() != nullptr)
				{
					s->path = chooser->OnChooserClosed();
					ReloadScript(s);
				}
			}
			if (ImGui::Button("Select Script"))
			{
				chooser->OpenPanel("LoadScript", "lua", { "lua" });
			}
			ImGui::SameLine();
			ImGui::Text(s->path.substr(s->path.find_last_of('/') + 1).c_str());

			bool isSeparatorNeeded = true;
			ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);
			for (std::vector<InspectorVariable*>::iterator variable = s->inspectorVariables.begin(); variable != s->inspectorVariables.end(); ++variable)
			{
				ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);

				if ((*variable)->type == INSPECTOR_NO_TYPE)
					continue;

				if ((*variable)->name == "")
				{
					// inspectorVariables.erase(variable);
					continue;
				}
				if (isSeparatorNeeded)
				{
					ImGui::Separator();
					isSeparatorNeeded = false;
				}

				switch ((*variable)->type)
				{
				case INSPECTOR_INT:
				{
					if (ImGui::DragInt((*variable)->name.c_str(), &std::get<int>((*variable)->value)))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<int>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_FLOAT:
				{
					if (ImGui::DragFloat((*variable)->name.c_str(), &std::get<float>((*variable)->value)))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<float>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_FLOAT2:
				{
					if (ImGui::DragFloat2((*variable)->name.c_str(), std::get<float2>((*variable)->value).ptr()))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<float2>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_FLOAT3:
				{
					if (ImGui::DragFloat3((*variable)->name.c_str(), std::get<float3>((*variable)->value).ptr()))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<float3>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_BOOL:
				{
					if (ImGui::Checkbox((*variable)->name.c_str(), &std::get<bool>((*variable)->value)))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<bool>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_STRING:
				{
					if (ImGui::InputText((*variable)->name.c_str(), &std::get<std::string>((*variable)->value)))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<std::string>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_TO_STRING:
				{
					ImGui::Text(std::get<std::string>((*variable)->value).c_str());
					break;
				}
				case INSPECTOR_FLOAT3_ARRAY:
				{
					int nWaypoints = std::get<std::vector<float3>>((*variable)->value).size();
					std::vector<float3> waypoints = std::get<std::vector<float3>>((*variable)->value);
					if (ImGui::DragInt("Path length", &nWaypoints, 1.0f, 0))
					{
						waypoints.clear();
						for (int i = 0; i < nWaypoints; i++)
						{
							waypoints.push_back(float3(0, 0, 0));
						}
						std::get<std::vector<float3>>((*variable)->value) = waypoints;
						s->handler->lua[(*variable)->name.c_str()] = waypoints;
					}

					ImGui::Text("Waypoints: ");
					for (int i = 0; i < nWaypoints; i++)
					{
						std::string label = std::to_string(i);
						if (ImGui::DragFloat3(label.c_str(), &(waypoints[i][0]), 0.5f))
						{
							std::get<std::vector<float3>>((*variable)->value)[i] = waypoints[i];
							s->handler->lua[(*variable)->name.c_str()] = waypoints;
						}
					}
					break;
				}
				case INSPECTOR_GAMEOBJECT:
				{
					GameObject* selected = std::get<GameObject*>((*variable)->value);
					std::string name = (selected == nullptr) ? "null" : selected->GetName();
					ImGui::InputText((*variable)->name.c_str(), &name);
					if (ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy");
						if (payload != nullptr)
						{
							GameObject* go = owner->GetEngine()->GetEditor()->GetPanelHierarchy()->GetSelectedGameObject();
							std::get<GameObject*>((*variable)->value) = go;
							s->handler->lua[(*variable)->name.c_str()] = go;
						}
						ImGui::EndDragDropTarget();
					}
					break;
				}
				}
				ImGui::PopID();
			}
			ImGui::PopID();

			if (!isSeparatorNeeded)
			{
				ImGui::Separator();
			}

			if (ImGui::Button("Reload Script"))
			{
				ReloadScript(s);
			}
			ImGui::PopID();
		}
	}
	else
	{
		DrawDeleteButton(owner, this);
	}

	return ret;
}

void ComponentScript::ReloadScript(ScriptHandler* handler)
{
	if (handler->path == "")
		return;
	handler->script = handler->handler->lua.script_file(handler->path);
	handler->isScriptLoaded = true;
}

void ComponentScript::Save(Json &json) const
{
	/*json["type"] = "script";
	json["file_name"] = path;
	json["script_number"] = numScript;
	Json jsonIV;
	for (InspectorVariable *variable : inspectorVariables)
	{
		switch (variable->type)
		{
		case INSPECTOR_INT:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "int";
			jsonIV["value"] = std::get<int>(variable->value);
		}
		break;
		case INSPECTOR_FLOAT:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "float";
			jsonIV["value"] = std::get<float>(variable->value);
		}
		break;
		case INSPECTOR_FLOAT2:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "float2";
			jsonIV["value"]["x"] = std::get<float2>(variable->value).x;
			jsonIV["value"]["y"] = std::get<float2>(variable->value).y;
		}
		break;
		case INSPECTOR_FLOAT3:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "float3";
			jsonIV["value"]["x"] = std::get<float3>(variable->value).x;
			jsonIV["value"]["y"] = std::get<float3>(variable->value).y;
			jsonIV["value"]["z"] = std::get<float3>(variable->value).z;
		}
		break;
		case INSPECTOR_BOOL:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "bool";
			jsonIV["value"] = std::get<bool>(variable->value);
		}
		break;
		case INSPECTOR_STRING:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "string";
			jsonIV["value"] = std::get<std::string>(variable->value);
		}
		break;
		case INSPECTOR_TO_STRING:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "to_string";
			jsonIV["value"] = std::get<std::string>(variable->value);
		}
		break;
		case INSPECTOR_FLOAT3_ARRAY:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "float3_array";
			std::vector<float> array;
			for (uint i = 0; i < std::get<std::vector<float3>>(variable->value).size(); ++i)
			{
				array.push_back(std::get<std::vector<float3>>(variable->value)[i].x);
				array.push_back(std::get<std::vector<float3>>(variable->value)[i].y);
				array.push_back(std::get<std::vector<float3>>(variable->value)[i].z);
			}
			json["value"] = array;
		}
		break;
		case INSPECTOR_GAMEOBJECT:
		{
			if (std::get<GameObject *>(variable->value) != nullptr)
			{
				jsonIV["name"] = variable->name;
				jsonIV["type"] = "gameObject";
				jsonIV["value"] = std::get<GameObject *>(variable->value)->GetUID();
			}
		}
		break;
		}
		json["inspector_variables"].push_back(jsonIV);
	}*/
}

void ComponentScript::Load(Json &json)
{
	/*path = json.at("file_name");
	numScript = json.at("script_number");
	LoadInspectorVariables(json);*/
}

void ComponentScript::LoadInspectorVariables(Json &json)
{
	/*if (!json.contains("inspector_variables"))
		return;
	for (const auto &var : json.at("inspector_variables").items())
	{
		std::string name = var.value().at("name").get<std::string>();
		std::string type_s = var.value().at("type").get<std::string>();
		INSPECTOR_VARIABLE_TYPE type = INSPECTOR_NO_TYPE;
		std::variant<int, float, float2, float3, bool, std::string, std::vector<float3>, GameObject *> value;

		if (type_s == "int")
		{
			type = INSPECTOR_INT;
			value = (int)var.value().at("value");
		}
		else if (type_s == "float")
		{
			type = INSPECTOR_FLOAT;
			value = (float)var.value().at("value");
		}
		else if (type_s == "float2")
		{
			type = INSPECTOR_FLOAT2;
			value = float2(var.value().at("value").at("x"), var.value().at("value").at("y"));
		}
		else if (type_s == "float3")
		{
			type = INSPECTOR_FLOAT3;
			value = float3(var.value().at("value").at("x"), var.value().at("value").at("y"), var.value().at("value").at("z"));
		}
		else if (type_s == "bool")
		{
			type = INSPECTOR_BOOL;
			value = (bool)var.value().at("value");
		}
		else if (type_s == "string")
		{
			type = INSPECTOR_STRING;
			value = (std::string)var.value().at("value");
		}
		else if (type_s == "to_string")
		{
			type = INSPECTOR_TO_STRING;
			value = (std::string)var.value().at("value");
		}
		else if (type_s == "float3_array")
		{
			type = INSPECTOR_FLOAT3_ARRAY;
			std::vector<float> array = json.at("value").get<std::vector<float>>();
			std::vector<float3> value;
			for (int i = 0; i < array.size(); i += 3)
			{
				float3 f3 = float3(array[i], array[i + 1], array[i + 2]);
				value.push_back(f3);
			}
		}
		else if (type_s == "gameObject")
		{
			type = INSPECTOR_GAMEOBJECT;
			value = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->GetGameObject((uint)var.value().at("value"));
		}

		InspectorVariable *variable = new InspectorVariable(name, type, value);
		inspectorVariables.push_back(variable);
	}*/
}

ScriptHandler::ScriptHandler(GameObject* owner)
{
	handler = new Scripting();
	handler->gameObject = owner;
	handler->componentTransform = owner->GetTransform();
	handler->SetUpVariableTypes();
}
