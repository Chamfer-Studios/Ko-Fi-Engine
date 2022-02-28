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


ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = ComponentType::SCRIPT;

	owner->numScripts++;
	numScript = owner->numScripts;

	//fileName = "Movement.lua"; // Temp

	componentTransform = owner->GetTransform();
}

ComponentScript::~ComponentScript()
{

}

bool ComponentScript::Start()
{
	bool ret = true;

	return ret;
}

bool ComponentScript::CleanUp()
{

	return true;
}

bool ComponentScript::Update(float dt)
{
	if (isRunning)
	{
		float x = owner->GetEngine()->GetScripting()->lua["Update"](dt, componentTransform->GetPosition().x, componentTransform->GetPosition().y, componentTransform->GetPosition().z, (int)owner->GetEngine()->GetInput()->GetMouseButton(1), (int)owner->GetEngine()->GetInput()->GetMouseButton(3));
		componentTransform->SetPosition(float3(x, componentTransform->GetPosition().y, componentTransform->GetPosition().z));

		//variables = owner->GetEngine()->GetScripting()->lua["ToShowInPanel"]();

	}
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
			if (ImGui::Button("Run")) // This will be an event call
			{
				script = owner->GetEngine()->GetScripting()->lua.load_file(fullName);
				script();
				isRunning = true;
			}
			if (ImGui::Button("Stop")) // This will be an event call
			{
				isRunning = false;
			}
		}
	}

	if (owner->GetEngine()->GetInput()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
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