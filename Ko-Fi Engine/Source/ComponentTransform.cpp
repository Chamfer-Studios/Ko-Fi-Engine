#include "ComponentTransform.h"
#include "PanelChooser.h"
#include "MathGeoLib/MathGeoLib.h"
#include "GameObject.h"
#include "Globals.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRANSFORM;

	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;

	transformMatrix.SetIdentity();
	transformMatrixLocal.SetIdentity();
}

ComponentTransform::~ComponentTransform()
{
}

bool ComponentTransform::Update()
{
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
	if (ImGui::CollapsingHeader("Transform"))
	{
		float3 newPosition = position;
		if (ImGui::DragFloat3("Location", &newPosition[0]))
		{
			SetPosition(newPosition);
		}
		float3 newRotationEuler;
		newRotationEuler.x = RADTODEG * rotationEuler.x;
		newRotationEuler.y = RADTODEG * rotationEuler.y;
		newRotationEuler.z = RADTODEG * rotationEuler.z;
		if (ImGui::DragFloat3("Rotation", &(newRotationEuler[0])))
		{
			newRotationEuler.x = DEGTORAD * newRotationEuler.x;
			newRotationEuler.y = DEGTORAD * newRotationEuler.y;
			newRotationEuler.z = DEGTORAD * newRotationEuler.z;
			SetRotation(newRotationEuler);
		}
		float3 newScale = scale;
		if (ImGui::DragFloat3("Scale", &(newScale[0])))
		{
			SetScale(newScale);
		}
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

void ComponentTransform::SetRotation(const Quat& newRotation)
{
	this->rotation = newRotation;
	rotationEuler = newRotation.ToEulerXYZ() * RADTODEG;
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

float4x4 ComponentTransform::GetGlobalTransform()
{
	return transformMatrix;
}

bool ComponentTransform::GetDirty()
{
	return isDirty;
}

void ComponentTransform::SetDirty(bool isDirty)
{
	this->isDirty = isDirty;
}

Json ComponentTransform::Save()
{
	Json jsonComponentTransform;

	if (this != nullptr)
	{
		jsonComponentTransform["transform_matrix"] = {
			transformMatrix.Col3(0).x,
			transformMatrix.Col3(0).y,
			transformMatrix.Col3(0).z,
			transformMatrix.Col3(1).x,
			transformMatrix.Col3(1).y,
			transformMatrix.Col3(1).z,
			transformMatrix.Col3(2).x,
			transformMatrix.Col3(2).y,
			transformMatrix.Col3(2).z,
			transformMatrix.Col3(3).x,
			transformMatrix.Col3(3).y,
			transformMatrix.Col3(3).z
		};
		jsonComponentTransform["transform_matrix_local"] = {
			transformMatrixLocal.Col3(0).x,
			transformMatrixLocal.Col3(0).y,
			transformMatrixLocal.Col3(0).z,
			transformMatrixLocal.Col3(1).x,
			transformMatrixLocal.Col3(1).y,
			transformMatrixLocal.Col3(1).z,
			transformMatrixLocal.Col3(2).x,
			transformMatrixLocal.Col3(2).y,
			transformMatrixLocal.Col3(2).z,
			transformMatrixLocal.Col3(3).x,
			transformMatrixLocal.Col3(3).y,
			transformMatrixLocal.Col3(3).z
		};

		jsonComponentTransform["is_dirty"] = GetDirty();

		jsonComponentTransform["position"] = {
			GetPosition().x,
			GetPosition().y,
			GetPosition().z
		};
		jsonComponentTransform["rotation"] = {
			GetRotation().x,
			GetRotation().y,
			GetRotation().z
		};
		jsonComponentTransform["scale"] = {
			GetScale().x,
			GetScale().y,
			GetScale().z
		};
	}

	return jsonComponentTransform;
}

void ComponentTransform::Load(Json jsonComponentTransform)
{
	std::vector<float> values = jsonComponentTransform["transform_matrix"].get<std::vector<float>>();
	float3 columnVector;
	for (int i = 0; i < 4; i++)
	{
		columnVector = { values[i * 3], values[(i * 3) + 1], values[(i * 3) + 2] };
		transformMatrix.SetCol3(i, columnVector);
	}

	values = jsonComponentTransform["transform_matrix_local"].get<std::vector<float>>();
	for (int i = 0; i < 4; i++)
	{
		columnVector = { values[i * 3], values[(i * 3) + 1], values[(i * 3) + 2] };
		transformMatrixLocal.SetCol3(i, columnVector);
	}

	SetDirty(jsonComponentTransform.at("is_dirty"));

	values = jsonComponentTransform["position"].get<std::vector<float>>();
	float3 position;
	position.x = values[0];
	position.y = values[1];
	position.z = values[2];
	SetPosition(position);

	values = jsonComponentTransform["rotation"].get<std::vector<float>>();
	float3 rotation;
	rotation.x = values[0];
	rotation.y = values[1];
	rotation.z = values[2];
	SetRotation(rotation);

	values = jsonComponentTransform["scale"].get<std::vector<float>>();
	float3 scale;
	scale.x = values[0];
	scale.y = values[1];
	scale.z = values[2];
	SetScale(scale);
}
