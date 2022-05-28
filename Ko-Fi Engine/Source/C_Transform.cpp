#include "C_Transform.h"
#include "Globals.h"

// Modules
#include "Engine.h"
#include "M_SceneManager.h"

// GameObject
#include "GameObject.h"
#include "C_Camera.h"
#include "C_Mesh.h"
#include "C_BoxCollider.h"
#include "C_SphereCollider.h"
#include "C_CapsuleCollider.h"


#include "PanelChooser.h"

#include "MathGeoLib/MathGeoLib.h"

C_Transform::C_Transform(GameObject *parent) : Component(parent)
{
	type = ComponentType::TRANSFORM;

	transformMatrixLocal.SetIdentity();
	transformMatrix = float4x4::FromTRS(float3::zero, Quat::identity, float3::one);
	isDirty = true;
}

C_Transform::~C_Transform()
{
	CleanUp();
}

bool C_Transform::Update(float dt)
{

	if (std::string(owner->GetName()) == std::string("harkonnen")) {
		int a = 0;
	}
	
	if (isDirty) // When Object is Modified
	{
		RecomputeGlobalMatrix();
		owner->PropagateTransform();

		if (owner->GetComponent<C_Mesh>())
		{
			if (owner->GetComponent<C_Mesh>()->GetMesh())
				owner->GetComponent<C_Mesh>()->GenerateGlobalBoundingBox();

			// Update colliders
			if (owner->GetComponent<C_BoxCollider>())
				owner->GetComponent<C_BoxCollider>()->UpdateScaleFactor();
			if (owner->GetComponent<C_SphereCollider>())
				owner->GetComponent<C_SphereCollider>()->UpdateScaleFactor();
			if (owner->GetComponent<C_CapsuleCollider>())
				owner->GetComponent<C_CapsuleCollider>()->UpdateScaleFactor();
		}

		isDirty = false;
	}

	return true;
}

bool C_Transform::PostUpdate(float dt)
{

	//glBegin(GL_LINES);
	//glColor3f(0, 0, 1.0f);
	//glLineWidth(4.f);
	//glVertex3f(GetPosition().x, GetPosition().y, GetPosition().z);
	//glVertex3f(GetPosition().x + Front().x * 50, GetPosition().y + Front().y * 50, GetPosition().z + Front().z * 50);
	//glEnd();

	return true;
}

bool C_Transform::CleanUp()
{
	return true;
}

bool C_Transform::InspectorDraw(PanelChooser *chooser)
{
	bool ret = true;
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Transform"))
	{

		// Position ImGui
		float3 newPosition = GetPosition();
		if (ImGui::DragFloat3("Location##", &(newPosition[0]), 0.5f))
		{
			SetPosition(newPosition);
		}

		// Rotation ImGui
		float3 newRotationEuler = GetRotationEuler();
		newRotationEuler = RadToDeg(newRotationEuler);
		if (ImGui::DragFloat3("Rotation##", &(newRotationEuler[0]), 0.045f))
		{
			newRotationEuler = DegToRad(newRotationEuler);
			SetRotationEuler(newRotationEuler);
		}

		// Scale ImGui
		float3 newScale = GetScale();
		if (ImGui::DragFloat3("Scale##", &(newScale[0]), 0.02f, 0.1f, 5000.f))
		{
			SetScale(newScale);
		}
	}

	return ret;
}

void C_Transform::SetPosition(const float3 &newPosition)
{
	transformMatrixLocal = float4x4::FromTRS(newPosition, GetRotationQuat(), GetScale());
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void C_Transform::SetScale(const float3 &newScale)
{
	float3 fixedScale = newScale;
	// If it is equal to 0 it crashes
	if (fixedScale.x == 0) fixedScale.x = 0.00001f;
	if (fixedScale.y == 0) fixedScale.y = 0.00001f;
	if (fixedScale.z == 0) fixedScale.z = 0.00001f;

	if (fixedScale.x <= 0) fixedScale.x *= -1.f;
	if (fixedScale.y <= 0) fixedScale.y *= -1.f;
	if (fixedScale.z <= 0) fixedScale.z *= -1.f;

	transformMatrixLocal = float4x4::FromTRS(GetPosition(), GetRotationQuat(), fixedScale);
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void C_Transform::SetRotationEuler(const float3 &newRotation)
{
	Quat rotation = Quat::FromEulerXYZ(newRotation.x, newRotation.y, newRotation.z);
	transformMatrixLocal = float4x4::FromTRS(GetPosition(), rotation, GetScale());
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void C_Transform::SetRotationQuat(const Quat &newRotation)
{
	transformMatrixLocal = float4x4::FromTRS(GetPosition(), newRotation, GetScale());
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void C_Transform::LookAt(float3 &_front, float3 &_up)
{
	_front = _front.Normalized();
	_up = _up.Normalized();

	float angle = atan2(_front.z, _front.x);

	Quat r = GetRotationQuat();

	float3 cross = _up.Cross(Up());
	float angleBetween = _up.AngleBetween(Up());

	r = r.RotateAxisAngle(cross, angleBetween);

	float3 currentEuler = r.ToEulerXYZ();

	float diff = currentEuler.y - angle;

	diff += 90.0f * DEGTORAD;

	r = r.RotateAxisAngle(_up, diff);

	SetRotationQuat(r);

	//transformMatrixLocal = float4x4::LookAt(GetPosition(), GetPosition() + _front, Front(), Up(), float3(0, 1, 0));
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->sceneTreeIsDirty = true;
	isDirty = true;
}

void C_Transform::SetGlobalTransform(const float4x4 &globalTransform)
{
	if (owner->GetParent() == nullptr) return;

	transformMatrixLocal = owner->GetParent()->GetTransform()->GetGlobalTransform().Inverted() * globalTransform;
	//transformMatrix = globalTransform;
	isDirty = true;
}

void C_Transform::SetDirty(bool isDirty)
{
	this->isDirty = isDirty;
}

float3 C_Transform::GetPosition() const
{
	float3 position = float3::zero;
	float3 scale = float3::zero;
	Quat rotation = Quat::identity;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return position;
}

float3 C_Transform::GetScale() const
{
	float3 position = float3::zero;
	float3 scale = float3::zero;
	Quat rotation = Quat::identity;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return scale;
}

float3 C_Transform::GetRotationEuler() const
{
	float3 position = float3::zero;
	float3 scale = float3::zero;
	Quat rotation = Quat::identity;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return rotation.ToEulerXYZ();
}

Quat C_Transform::GetRotationQuat() const
{
	float3 position = float3::zero;
	float3 scale = float3::zero;
	Quat rotation = Quat::identity;
	transformMatrixLocal.Decompose(position, rotation, scale);
	return rotation;
}

const float3 C_Transform::Right() const
{
	return transformMatrixLocal.Col3(0).Normalized();
}

const float3 C_Transform::GlobalRight() const
{
	return transformMatrix.Col3(0).Normalized();
}

const float3 C_Transform::Up() const
{
	return transformMatrixLocal.Col3(1).Normalized();
}

const float3 C_Transform::GlobalUp() const
{
	return transformMatrix.Col3(1).Normalized();
}

const float3 C_Transform::Front() const
{
	return transformMatrixLocal.Col3(2).Normalized();
}

const float3 C_Transform::GlobalFront() const
{
	return transformMatrix.Col3(2).Normalized();
}

void C_Transform::RecomputeGlobalMatrix()
{

	if (owner->GetParent() != nullptr && owner->GetParent()->GetComponent<C_Transform>())
	{
		transformMatrix = owner->GetParent()->GetTransform()->transformMatrix.Mul(transformMatrixLocal);
	}
	else
	{
		transformMatrix = transformMatrixLocal;
	}
}

void C_Transform::Save(Json &json) const
{
	json["type"] = (int)type;

	float3 position = GetPosition();
	float3 scale = GetScale();
	Quat rotation = GetRotationQuat();

	json["position"] = {position.x, position.y, position.z};
	json["rotation"] = {rotation.x, rotation.y, rotation.z, rotation.w};
	json["scale"] = {scale.x, scale.y, scale.z};
}

void C_Transform::Load(Json &json)
{
	std::vector<float> values = json.at("position").get<std::vector<float>>();
	SetPosition(float3(values[0], values[1], values[2]));
	values.clear();
	values.shrink_to_fit();;

	values = json.at("rotation").get<std::vector<float>>();
	SetRotationQuat(Quat(values[0], values[1], values[2], values[3]));
	values.clear();
	values.shrink_to_fit();;

	values = json.at("scale").get<std::vector<float>>();
	SetScale(float3(values[0], values[1], values[2]));
	values.clear();
	values.shrink_to_fit();

	transformMatrixLocal = float4x4::FromTRS(GetPosition(), GetRotationQuat(), GetScale());

	RecomputeGlobalMatrix();
	owner->PropagateTransform();
}