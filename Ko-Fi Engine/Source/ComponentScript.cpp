#include "ComponentScript.h"
#include "Engine.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "Globals.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "imgui_stdlib.h"


ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = ComponentType::SCRIPT;

	fileName = "Movement.lua"; // Temp
	std::string fullName = "../Source/" + fileName;

	// Load Script without running
	script1 = parent->GetEngine()->GetScripting()->lua.load_file(fullName);

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

bool ComponentScript::Update()
{
	
	return true;
}

bool ComponentScript::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.
	
	if (ImGui::CollapsingHeader("Script"))
	{
		ImGui::InputText("Script Name", &(fileName));
		if (ImGui::Button("Run"))
		{
			script1();
		}
	}

	return ret;
}