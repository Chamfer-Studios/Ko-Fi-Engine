#include "ComponentTransform.h"
#include "PanelChooser.h"
#include "MathGeoLib/MathGeoLib.h"
#include "GameObject.h"
ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent) {

	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;

	transformMatrix.SetIdentity();
	transformMatrixLocal.SetIdentity();
}

ComponentTransform::~ComponentTransform()
{
}
bool ComponentTransform::Update() {
	if (isDirty)
	{
		transformMatrixLocal = float4x4::FromTRS(position, rotation, scale);

		right = transformMatrixLocal.Col3(0).Normalized();
		up = transformMatrixLocal.Col3(1).Normalized();
		front = transformMatrixLocal.Col3(2).Normalized();
		RecomputeGlobalMatrix();
		owner->PropagateTransform();
		isDirty = false;
	}
	return true;
}

bool ComponentTransform::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Transform")) {
		float3 newPosition = position;
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("X##positionX", &newPosition.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y##positionY", &newPosition.y);
		ImGui::SameLine();
		ImGui::InputFloat("Z##positionZ", &newPosition.z);
		ImGui::SameLine();
		ImGui::Text("Position");
		ImGui::PopItemWidth();
		float3 newRotationEuler;
		newRotationEuler.x = RadToDeg(rotationEuler.x);
		newRotationEuler.y = RadToDeg(rotationEuler.y);
		newRotationEuler.z = RadToDeg(rotationEuler.z);
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("X##rotationX", &newRotationEuler.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y##rotationY", &newRotationEuler.y);
		ImGui::SameLine();
		ImGui::InputFloat("Z##rotationZ", &newRotationEuler.z);
		ImGui::SameLine();
		ImGui::Text("Rotation");
		ImGui::PopItemWidth();
		float3 newScale = scale;
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("X##scaleX", &newScale.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y##scaleY", &newScale.y);
		ImGui::SameLine();
		ImGui::InputFloat("Z##scaleZ", &newScale.z);
		ImGui::SameLine();
		ImGui::Text("Scale");
		ImGui::PopItemWidth();
		SetPosition(newPosition);
		SetRotation(newRotationEuler);
		SetScale(newScale);
	}
	
	return ret;
}

void ComponentTransform::SetPosition(const float3& newPosition)
{
	position = newPosition;
	isDirty = true;
}

void ComponentTransform::SetRotation(const float3& newRotation)
{
	Quat rotationDelta = Quat::FromEulerXYZ(newRotation.x - rotationEuler.x, newRotation.y - rotationEuler.y, newRotation.z - rotationEuler.z);
	rotation = rotation*rotationDelta;
	rotationEuler = newRotation;
	isDirty = true;
}

void ComponentTransform::SetScale(const float3& newScale)
{
	scale = newScale;
	isDirty = true;
}

void ComponentTransform::NewAttachment()
{
	if (owner->GetParent() != nullptr)
		transformMatrixLocal = owner->GetParent()->GetTransform()->transformMatrix.Inverted().Mul(transformMatrix);

	float3x3 rot;
	transformMatrixLocal.Decompose(position, rot, scale);
	rotationEuler = rot.ToEulerXYZ();
}

void ComponentTransform::OnParentMoved()
{
	RecomputeGlobalMatrix();
}

void ComponentTransform::RecomputeGlobalMatrix()
{
	if (owner->GetParent() != nullptr)
	{
		transformMatrix = owner->GetParent()->GetTransform()->transformMatrix.Mul(transformMatrixLocal);
	}
	else
	{
		transformMatrix = transformMatrixLocal;
	}
}