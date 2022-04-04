#include "ComponentTransform.h"

#include "Engine.h"
#include "SceneManager.h"
#include "Globals.h"

#include "PanelChooser.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "C_Collider.h"

#include "MathGeoLib/MathGeoLib.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRANSFORM;

	transformMatrixLocal.SetIdentity();
	transformMatrix = float4x4::FromTRS(float3::zero, Quat::identity, float3::one);
	isDirty = true;
}

ComponentTransform::~ComponentTransform()
{}

bool ComponentTransform::Update(float dt)
{
	if (isDirty) // When Object is Modified
	{
		RecomputeGlobalMatrix();
		owner->PropagateTransform();
		if (owner->GetComponent<ComponentCollider2>())
			owner->GetComponent<ComponentCollider2>()->UpdateCollSizeFromAABB();
		isDirty = false;
	}

	return true;
}

bool ComponentTransform::CleanUp()
{
	return true;
}

bool ComponentTransform::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Transform"))
	{

		// Position ImGui
		float3 newPosition = GetPosition();
		if (ImGui::DragFloat3("Location", &newPosition[0]), 0.005f)
		{
			SetPosition(newPosition);
		}

		// Rotation ImGui
		float3 newRotationEuler = GetRotationEuler();
		newRotationEuler = RadToDeg(newRotationEuler);
		if (ImGui::DragFloat3("Rotation", &(newRotationEuler[0]), 0.045f))
		{
			newRotationEuler = DegToRad(newRotationEuler);
			SetRotationEuler(newRotationEuler);
		}

		// Scale ImGui
		float3 newScale = GetScale();
		if (ImGui::DragFloat3("Scale", &(newScale[0]), 0.02f, 0.00000001f, 5000.f))
		{
			SetScale(newScale);
		}
	}

	return ret;
}

void ComponentTransform::SetPosition(const float3& newPosition)
{
	transformMatrixLocal = float4x4::FromTRS(newPosition, GetRotationQuat(), GetScale());
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void ComponentTransform::SetScale(const float3& newScale)
{
	float3 fixedScale = newScale;
	// If it is equal to 0 it crashes
	if (fixedScale.x == 0) fixedScale.x = 0.1f;
	if (fixedScale.y == 0) fixedScale.y = 0.1f;
	if (fixedScale.z == 0) fixedScale.z = 0.1f;

	if (fixedScale.x <= 0) fixedScale.x *= -1.f;
	if (fixedScale.y <= 0) fixedScale.y *= -1.f;
	if (fixedScale.z <= 0) fixedScale.z *= -1.f;

	transformMatrixLocal = float4x4::FromTRS(GetPosition(), GetRotationQuat(), fixedScale);
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void ComponentTransform::SetRotationEuler(const float3& newRotation)
{
	Quat rotation = Quat::FromEulerXYZ(newRotation.x, newRotation.y, newRotation.z);
	transformMatrixLocal = float4x4::FromTRS(GetPosition(), rotation, GetScale());
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void ComponentTransform::SetRotationQuat(const Quat& newRotation)
{
	transformMatrixLocal = float4x4::FromTRS(GetPosition(), newRotation, GetScale());
	isDirty = true;
}

void ComponentTransform::SetFront(const float3& front)
{
	transformMatrixLocal.SetCol3(2, front);

}

void ComponentTransform::SetGlobalTransform(const float4x4& globalTransform)
{
	if (owner->GetParent() == nullptr) return;
	transformMatrixLocal = owner->GetParent()->GetTransform()->GetGlobalTransform().Inverted() * globalTransform;
	transformMatrix = globalTransform;
	isDirty = true;
}

void ComponentTransform::SetDirty(bool isDirty)
{
	this->isDirty = isDirty;
}

float3 ComponentTransform::GetPosition() const
{
	float3 position;
	float3 scale;
	Quat rotation;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return position;
}

float3 ComponentTransform::GetScale() const
{
	float3 position;
	float3 scale;
	Quat rotation;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return scale;
}

float3 ComponentTransform::GetRotationEuler() const
{
	float3 position;
	float3 scale;
	Quat rotation;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return rotation.ToEulerXYZ();
}

Quat ComponentTransform::GetRotationQuat() const
{
	float3 position;
	float3 scale;
	Quat rotation;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return rotation;
}

const float3& ComponentTransform::Right() const
{
	return transformMatrixLocal.Col3(0).Normalized();
}

const float3& ComponentTransform::Up() const
{
	return transformMatrixLocal.Col3(1).Normalized();
}

const float3& ComponentTransform::Front() const
{
	return transformMatrixLocal.Col3(2).Normalized();
}

float4x4 ComponentTransform::GetGlobalTransform()
{
	return transformMatrix;
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

void ComponentTransform::Save(Json& json) const
{
	float3 position = GetPosition();
	float3 scale = GetScale();
	Quat rotation = GetRotationQuat();

	json["type"] = "transform";
	json["position"] = { position.x,position.y,position.z };
	json["rotation"] = { rotation.x,rotation.y,rotation.z,rotation.w };
	json["scale"] = { scale.x,scale.y,scale.z };
}

void ComponentTransform::Load(Json& json)
{
	std::vector<float> values = json.at("position").get<std::vector<float>>();
	SetPosition(float3(values[0], values[1], values[2]));
	values.clear();

	values = json.at("rotation").get<std::vector<float>>();
	SetRotationQuat(Quat(values[0], values[1], values[2], values[3]));
	values.clear();

	values = json.at("scale").get<std::vector<float>>();
	SetScale(float3(values[0], values[1], values[2]));
	values.clear();

	transformMatrixLocal = float4x4::FromTRS(GetPosition(), GetRotationQuat(), GetScale());

	RecomputeGlobalMatrix();
	owner->PropagateTransform();
}