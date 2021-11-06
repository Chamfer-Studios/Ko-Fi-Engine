#include "ComponentTransform.h"
ComponentTransform::ComponentTransform() : Component(COMPONENT_TYPE::COMPONENT_TRANSFORM)
{
	position = Vector();
	rotation = Vector();
	scale = Vector();
}

ComponentTransform::~ComponentTransform()
{
}

bool ComponentTransform::InspectorDraw()
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::PushItemWidth(50);
		ImGui::SliderFloat("", & position.x, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("", & position.y, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("", & position.z, 0, 120);
		ImGui::SameLine();
		ImGui::Text("Position");
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(50);
		ImGui::SliderFloat("", &rotation.x, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("", &rotation.y, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("", &rotation.z, 0, 120);
		ImGui::SameLine();
		ImGui::Text("Rotation");
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(50);
		ImGui::SliderFloat("", &scale.x, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("", &scale.y, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("", &scale.z, 0, 120);
		ImGui::SameLine();
		ImGui::Text("Scale");
		ImGui::PopItemWidth();
	}
	return ret;
}


