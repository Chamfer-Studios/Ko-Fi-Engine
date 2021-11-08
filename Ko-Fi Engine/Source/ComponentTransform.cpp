#include "ComponentTransform.h"
#include "PanelChooser.h"
#include "MathGeoLib/MathGeoLib.h"
ComponentTransform::ComponentTransform(GameObject* owner) : Component(COMPONENT_TYPE::COMPONENT_TRANSFORM)
{
	position.x = position.y = position.z = 0;
	rotation.x = rotation.y = rotation.z = 0;
	scale.x = scale.y = scale.z = 1;
	transform = IdentityMatrix;
	this->owner = owner;
}

ComponentTransform::~ComponentTransform()
{
}

bool ComponentTransform::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("X##positionX", &position.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y##positionY", &position.y);
		ImGui::SameLine();
		ImGui::InputFloat("Z##positionZ", &position.z);
		ImGui::SameLine();
		ImGui::Text("Position");
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("X##rotationX", &rotation.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y##rotationY", &rotation.y);
		ImGui::SameLine();
		ImGui::InputFloat("Z##rotationZ", &rotation.z);
		ImGui::SameLine();
		ImGui::Text("Rotation");
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("X##scaleX", &scale.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y##scaleY", &scale.y);
		ImGui::SameLine();
		ImGui::InputFloat("Z##scaleZ", &scale.z);
		ImGui::SameLine();
		ImGui::Text("Scale");
		ImGui::PopItemWidth();
	}
	SetScale(scale.x, scale.y, scale.z);
	SetPosition(position.x,position.y,position.z);
	SetRotation(rotation.x, rotation.y, rotation.z);
	return ret;
}

void ComponentTransform::SetPosition(float x,float y, float z)
{
	transform.translate(x, y, z);
}

void ComponentTransform::SetRotation(float x, float y, float z)
{
	Quat rotation(0,0,0,0);
	rotation = Quat().FromEulerXYZ(x,y ,z );
	float angle = 2 * acos(rotation.w);
	vec3 axis(0, 0, 0);
	axis.x = rotation.x / sqrt(1 - rotation.w * rotation.w);
	axis.y = rotation.y / sqrt(1 - rotation.w * rotation.w);
	axis.z = rotation.z / sqrt(1 - rotation.w * rotation.w);
	transform.rotate(angle,axis);

}

void ComponentTransform::SetScale(float x, float y, float z)
{
	transform.scale(x, y, z);

}

vec3 ComponentTransform::GetPosition()
{
	return position;
}

vec3 ComponentTransform::GetRotation()
{
	return rotation;
}

vec3 ComponentTransform::GetScale()
{
	return scale;
}

float* ComponentTransform::GetTransformMatrix()
{
	return transform.M;
}
