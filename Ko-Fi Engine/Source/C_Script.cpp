#include "C_Script.h"
#include "Scripting.h"

// General
#include "Engine.h"
#include "Globals.h"
#include "Log.h"

// Modules
#include "M_Input.h"
#include "M_Editor.h"

// GameObject
#include "GameObject.h"
#include "C_Transform.h"

// Panels
#include "PanelInspector.h"
#include "PanelChooser.h"
#include "PanelHierarchy.h"

// Extra
#include "imgui_stdlib.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float2.h"
#include "ImGuiAppLog.h"
#include "RNG.h"

#include <fstream>
#include <vector>

C_Script::C_Script(GameObject *parent) : Component(parent)
{
	type = ComponentType::SCRIPT;
	SetId(RNG::GetRandomUint());
	s = new ScriptHandler(owner, this);
}

C_Script::~C_Script()
{
	CleanUp();
}

bool C_Script::Start()
{
	bool ret = true;
	return ret;
}

bool C_Script::CleanUp()
{
	if(s != nullptr)
	{
		s->handler->CleanUp();
		s->inspectorVariables.clear();
		s->inspectorVariables.shrink_to_fit();
		delete s;
	}

	s = nullptr;
	
	return true;
}

bool C_Script::Update(float dt)
{
	if (s != nullptr)
	{
		while (eventQueue.size() != 0) {
			auto e = eventQueue.front();
			eventQueue.pop();

			auto receiver = sol::protected_function(s->handler->lua["EventHandler"]);

			if (receiver.valid()) {
				sol::protected_function_result result = receiver(e.key, e.fields);
				if (result.valid()) {
					// Call succeeded
				}
				else {
					// Call failed
					sol::error err = result;
					std::string what = err.what();
					appLog->AddLog("%s\n", what.c_str());
				}
			}
		}

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
			}
		}
	}
	
	return true;
}

bool C_Script::PostUpdate(float dt)
{
	if(s != nullptr)
	{
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

bool C_Script::OnPlay()
{
	bool ret = true;

	if (s != nullptr)
	{
		if (owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PLAYING && s->isScriptLoaded)
		{
			auto f = s->handler->lua["Start"];

			if (f.valid()) {
				f();
			}
		}
	}

	return ret;
}

bool C_Script::InspectorDraw(PanelChooser *chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	ImGui::PushID(std::to_string(id).c_str());
	std::string headerTypename = "Script";
	std::string number = std::to_string(id);
	std::string headerName = headerTypename.append(number);

	if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		if (chooser->IsReadyToClose("Add Script_" + std::to_string(id)))
		{
			if (chooser->OnChooserClosed() != nullptr)
			{
				std::string path = chooser->OnChooserClosed();

				if (!path.empty())
				{
					this->CleanUp();
					ScriptHandler* handler = new ScriptHandler(owner, this);
					s = handler;
					handler->path = path;
					ReloadScript(handler);
					
				}
			}
		}

		ImGui::Spacing();

		if (ImGui::Button("Add Script"))
		{
			chooser->OpenPanel("Add Script_" + std::to_string(id), "lua", {"lua"});
		}

		if (s != nullptr)
		{
			ImGui::Text(s->path.substr(s->path.find_last_of('/') + 1).c_str());
		}

		bool isSeparatorNeeded = true;

		if(s != nullptr)
		{
			for (std::vector<InspectorVariable*>::iterator variable = s->inspectorVariables.begin(); variable != s->inspectorVariables.end(); ++variable)
			{
				if ((*variable)->type == INSPECTOR_NO_TYPE)
				{
					continue;
				}


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

				std::string sLabel = ((*variable)->name);
				const char* label = sLabel.c_str();

				switch ((*variable)->type)
				{
				case INSPECTOR_INT:
				{
					if (ImGui::DragInt(label, &std::get<int>((*variable)->value)))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<int>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_FLOAT:
				{
					if (ImGui::DragFloat(label, &std::get<float>((*variable)->value)))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<float>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_FLOAT2:
				{
					if (ImGui::DragFloat2(label, std::get<float2>((*variable)->value).ptr()))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<float2>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_FLOAT3:
				{
					if (ImGui::DragFloat3(label, std::get<float3>((*variable)->value).ptr()))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<float3>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_BOOL:
				{
					if (ImGui::Checkbox(label, &std::get<bool>((*variable)->value)))
					{
						s->handler->lua[(*variable)->name.c_str()] = std::get<bool>((*variable)->value);
					}
					break;
				}
				case INSPECTOR_STRING:
				{
					if (ImGui::InputText(label, &std::get<std::string>((*variable)->value)))
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
						std::string _label = std::to_string(i);
						if (ImGui::DragFloat3(_label.c_str(), &(waypoints[i][0]), 0.5f))
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
			}
		}
		
		if (!isSeparatorNeeded)
		{
			ImGui::Separator();
		}

		if (ImGui::Button("Reload Script"))
		{
			ReloadScript(s);
		}
	}
	
	else
		DrawDeleteButton(owner, this);

	ImGui::PopID();
	return ret;
}

void C_Script::ReloadScript(ScriptHandler* handler)
{
	if (handler->path == "")
		return;

	//inspectorVariables.clear();
	//inspectorVariables.shrink_to_fit();
	//script = handler->lua.load_file(path);
	//script();

	handler->script = handler->handler->lua.script_file(handler->path);
	handler->isScriptLoaded = true;
}

void C_Script::Save(Json &json) const
{
	json["type"] = "script";
	json["id"] = id;
	json["file_name"] = s->path;
	Json jsonIV;
	for (InspectorVariable *variable : s->inspectorVariables)
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
			jsonIV["value"] = array;
		}
		break;
		case INSPECTOR_GAMEOBJECT:
		{
			jsonIV["name"] = variable->name;
			jsonIV["type"] = "gameObject";
			if (std::get<GameObject*>(variable->value) != nullptr)
			{
				jsonIV["value"] = std::get<GameObject *>(variable->value)->GetUID();
			}
			else {
				jsonIV["value"] = 0;
			}
		}
		break;
		}
		json["inspector_variables"].push_back(jsonIV);
	}
}

void C_Script::Load(Json &json)
{
	s->path = json.at("file_name");
	if (json.find("id") != json.end()) {
		id = json.at("id");
	}
	else {
		SetId(RNG::GetRandomUint());
	}
	LoadInspectorVariables(json);
	ReloadScript(s);
}

void C_Script::SetId(int id)
{
	this->id = id;
}


void C_Script::LoadInspectorVariables(Json &json)
{
	if (!json.contains("inspector_variables"))
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
			std::vector<float> array = var.value().at("value");
			std::vector<float3> value;
			for (int i = 0; i < array.size(); i += 3)
			{
				float3 f3 = float3(array[i], array[i + 1], array[i + 2]);
				value.push_back(f3);
			}
			InspectorVariable* variable = new InspectorVariable(name, type, value);
			s->inspectorVariables.push_back(variable);
			continue;
		}
		else if (type_s == "gameObject")
		{
			type = INSPECTOR_GAMEOBJECT;
			uint uid = (uint)var.value().at("value");
			value = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->GetGameObject(uid);
		}

		InspectorVariable *variable = new InspectorVariable(name, type, value);
		s->inspectorVariables.push_back(variable);
	}
}

ScriptHandler::ScriptHandler(GameObject* owner, C_Script* script)
{
	handler = new Scripting(script);
	handler->gameObject = owner;
	handler->componentTransform = owner->GetTransform();
	handler->SetUpVariableTypes();
}
