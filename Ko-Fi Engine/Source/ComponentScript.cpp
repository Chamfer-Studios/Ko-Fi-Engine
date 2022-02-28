#include "ComponentScript.h"
#include "Engine.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "Input.h"

#include "Globals.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "imgui_stdlib.h"


ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = ComponentType::SCRIPT;

	fileName = "Movement.lua"; // Temp

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
		bool isMouseLeftClicked = owner->GetEngine()->GetInput()->GetMouseButton(1) ? KEY_STATE::KEY_REPEAT : false;
		bool isMouseRightClicked = owner->GetEngine()->GetInput()->GetMouseButton(3) ? KEY_STATE::KEY_DOWN : false;
		//isRunning = false;
		float x = owner->GetEngine()->GetScripting()->lua["Update"](dt, componentTransform->GetPosition().x, componentTransform->GetPosition().y, componentTransform->GetPosition().z, isMouseLeftClicked, isMouseRightClicked);
		componentTransform->SetPosition(float3(x, componentTransform->GetPosition().y, componentTransform->GetPosition().z));
	}
	return true;
}

bool ComponentScript::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.
	
	if (ImGui::CollapsingHeader("Script"))
	{
		ImGui::InputText("Script Name", &(fileName));
		if (ImGui::Button("Run")) // This will be an event call
		{
			std::string fullName = "../Source/" + fileName;
			script = owner->GetEngine()->GetScripting()->lua.load_file(fullName);
			script();
			isRunning = true;
		}
		if (ImGui::Button("Stop")) // This will be an event call
		{
			isRunning = false;
		}
	}

	return ret;
}