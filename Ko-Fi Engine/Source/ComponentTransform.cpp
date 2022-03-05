#include "ComponentTransform.h"
#include "PanelChooser.h"
#include "MathGeoLib/MathGeoLib.h"
#include "GameObject.h"
#include "Globals.h"
#include "Engine.h"
#include "SceneManager.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRANSFORM;

	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;
	isDirty = true;

	transformMatrix.SetIdentity();
	transformMatrixLocal.SetIdentity();
}

ComponentTransform::~ComponentTransform()
{
}

bool ComponentTransform::Update(float dt)
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
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void ComponentTransform::SetRotation(const float3& newRotation)
{
	Quat rotationDelta = Quat::FromEulerXYZ(newRotation.x - rotationEuler.x, newRotation.y - rotationEuler.y, newRotation.z - rotationEuler.z);
	rotation = rotation*rotationDelta;
	rotationEuler = newRotation;
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void ComponentTransform::SetRotationQuat(const Quat& newRotation)
{
	this->rotation = newRotation;
	rotationEuler = newRotation.ToEulerXYZ() * RADTODEG;
	isDirty = true;
}

void ComponentTransform::SetScale(const float3& newScale)
{
	scale = newScale;
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
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

void ComponentTransform::SetGlobalTransform(const float4x4& globalTransform)
{
	transformMatrix = globalTransform;
}

bool ComponentTransform::GetDirty() const
{
	return isDirty;
}

void ComponentTransform::SetDirty(bool isDirty)
{
	this->isDirty = isDirty;
}

void ComponentTransform::Save(Json& json) const
{
	json["type"] = "transform";
	json["position"] = { position.x,position.y,position.z };
	json["rotation"] = { rotation.x,rotation.y,rotation.z,rotation.w };
	json["scale"] = { scale.x,scale.y,scale.z };
}

void ComponentTransform::Load(Json& json)
{
	std::vector<float> values = json["position"].get<std::vector<float>>();
	SetPosition(float3(values[0], values[1], values[2]));
	values.clear();

	values = json["rotation"].get<std::vector<float>>();
	SetRotationQuat(Quat(values[0], values[1], values[2], values[3]));
	values.clear();

	values = json["scale"].get<std::vector<float>>();
	SetScale(float3(values[0], values[1], values[2]));
	values.clear();
}