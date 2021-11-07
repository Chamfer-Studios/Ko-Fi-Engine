#include "ComponentInfo.h"
#include "GameObject.h"
#include "imgui_stdlib.h"

ComponentInfo::ComponentInfo(GameObject* owner) : Component(COMPONENT_TYPE::COMPONENT_INFO)
{
	this->owner = owner;
}

ComponentInfo::~ComponentInfo()
{
}

bool ComponentInfo::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Info")) {
		ImGui::Text("Name:");
		ImGui::SameLine();
		ImGui::InputText("##Name",&(owner->name));
		ImGui::Checkbox("Active", &owner->active);
	}
	return ret;
}
