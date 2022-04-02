#include "ComponentInfo.h"
#include "GameObject.h"
#include "imgui_stdlib.h"
#include "PanelChooser.h"

ComponentInfo::ComponentInfo(GameObject* parent) : Component(parent)
{
	type = ComponentType::INFO;
}

ComponentInfo::~ComponentInfo()
{}

bool ComponentInfo::CleanUp()
{
	return true;
}

bool ComponentInfo::Update(float dt)
{
	return true;
}

bool ComponentInfo::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Info"))
	{
		ImGui::Text("Name:");
		ImGui::SameLine();
		std::string newName = owner->GetName();
		if (ImGui::InputText("##Name", &(newName)))
		{
			owner->SetName(newName.c_str());
		}
		ImGui::Checkbox("Active", &owner->active);
	}

	return ret;
}

void ComponentInfo::Save(Json& json) const
{
	json["type"] = "info";
}

void ComponentInfo::Load(Json& json)
{
}