#include "ComponentTransform.h"
ComponentTransform::ComponentTransform() : Component(COMPONENT_TYPE::COMPONENT_TRANSFORM)
{
	position.x = position.y = position.z = 0;
	rotation.x = rotation.y = rotation.z = 0;
	scale.x = scale.y = scale.z = 0;
}

ComponentTransform::~ComponentTransform()
{
}

bool ComponentTransform::InspectorDraw()
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::PushItemWidth(50);
		ImGui::SliderFloat("##positionX", &position.x, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("##positionY", &position.y, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("##positionZ", &position.z, 0, 120);
		ImGui::SameLine();
		ImGui::Text("Position");
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(50);
		ImGui::SliderFloat("##rotationX", &rotation.x, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("##rotationY", &rotation.y, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("##rotationZ", &rotation.z, 0, 120);
		ImGui::SameLine();
		ImGui::Text("Rotation");
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(50);
		ImGui::SliderFloat("##scaleX", &scale.x, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("##scaleY", &scale.y, 0, 120);
		ImGui::SameLine();
		ImGui::SliderFloat("##scaleZ", &scale.z, 0, 120);
		ImGui::SameLine();
		ImGui::Text("Scale");
		ImGui::PopItemWidth();
	}
	return ret;
}


