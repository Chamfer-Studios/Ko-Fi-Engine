#include "ComponentInfo.h"
#include "GameObject.h"
#include "imgui_stdlib.h"
#include "PanelChooser.h"

ComponentInfo::ComponentInfo(GameObject* parent) : Component(parent)
{
	type = ComponentType::INFO;
}

ComponentInfo::~ComponentInfo()
{
}

bool ComponentInfo::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Info"))
	{
		ImGui::Text("Name:");
		ImGui::SameLine();
		ImGui::InputText("##Name",&(owner->name));
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