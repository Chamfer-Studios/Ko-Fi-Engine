#include "C_RigidBody.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "M_SceneManager.h"
#include "C_Transform.h"


C_RigidBody::C_RigidBody(GameObject* parent) : Component(parent)
{
	type = ComponentType::RIGID_BODY2;
}

C_RigidBody::~C_RigidBody()
{
	CleanUp();
}

bool C_RigidBody::Start()
{
	float3 gameObjectPosition = owner->GetTransform()->GetPosition();
	Quat gameObjectOrientation = owner->GetTransform()->GetRotationQuat();
	reactphysics3d::Vector3 rbPosition(gameObjectPosition.x, gameObjectPosition.y, gameObjectPosition.z);
	reactphysics3d::Quaternion rbOrientation(gameObjectOrientation.x, gameObjectOrientation.y, gameObjectOrientation.z, gameObjectOrientation.w);
	reactphysics3d::Transform rbTransform(rbPosition, rbOrientation);
	body = owner->GetEngine()->GetPhysics()->GetWorld()->createRigidBody(rbTransform);
	return true;
}

bool C_RigidBody::Update(float dt)
{
	if (hasUpdated)
		UpdateRB();

	if (owner->GetEngine()->GetSceneManager()->GetGameState() != GameState::PLAYING || body->getType() == reactphysics3d::BodyType::STATIC)
	{
		//Transform updates the rigid body
		float3 gameObjectPosition = owner->GetTransform()->GetPosition();
		Quat gameObjectOrientation = owner->GetTransform()->GetRotationQuat();
		reactphysics3d::Vector3 rbPosition(gameObjectPosition.x, gameObjectPosition.y, gameObjectPosition.z);
		reactphysics3d::Quaternion rbOrientation(gameObjectOrientation.x, gameObjectOrientation.y, gameObjectOrientation.z, gameObjectOrientation.w);
		reactphysics3d::Transform rbTransform(rbPosition, rbOrientation);
		body->setTransform(rbTransform);
	}
	else
	{
		//rigid body updates the transform
		const reactphysics3d::Transform& transform = body->getTransform();
		const reactphysics3d::Vector3& position = transform.getPosition();
		const reactphysics3d::Quaternion& orientation = transform.getOrientation();

		owner->GetTransform()->SetPosition(float3(position.x, position.y, position.z));
		owner->GetTransform()->SetRotationQuat(Quat(orientation.x, orientation.y, orientation.z, orientation.w));
	}


	return true;
}

bool C_RigidBody::CleanUp()
{
	// Destroy a rigid body 
	owner->GetEngine()->GetPhysics()->GetWorld()->destroyRigidBody(body);
	return true;
}

bool C_RigidBody::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		std::string newBodyType = GetBodyType();
		if (ImGui::BeginCombo("Set Type##", newBodyType.c_str()))
		{
			if (ImGui::Selectable("Static"))
			{
				newBodyType = "Static";
				SetBodyType(newBodyType);
			}
			if (ImGui::Selectable("Dynamic"))
			{
				newBodyType = "Dynamic";
				SetBodyType(newBodyType);
			}
			if (ImGui::Selectable("Kinematic"))
			{
				newBodyType = "Kinematic";
				SetBodyType(newBodyType);
			}
			ImGui::EndCombo();
		}

		bool newUseGravity = GetUseGravity();
		if (ImGui::Checkbox("Use Gravity##", &newUseGravity))
		{
			SetUseGravity(newUseGravity);
		}

		ImGui::Text("Mass");
		ImGui::SameLine();
		float newMass = GetMass();
		if (ImGui::DragFloat("##mass", &newMass, 0.01f, 0.01f, 20.0f))
			SetMass(newMass);

		if (ImGui::TreeNodeEx("Constraints"))
		{
			bool newFreezePositionX = GetFreezePositionX();
			bool newFreezePositionY = GetFreezePositionY();
			bool newFreezePositionZ = GetFreezePositionZ();
			ImGui::Text("Freeze position: ");
			if (ImGui::Checkbox("X##FreezePosX", &newFreezePositionX))
				FreezePositionX(newFreezePositionX);
			ImGui::SameLine();
			if (ImGui::Checkbox("Y##FreezePosY", &newFreezePositionY))
				FreezePositionY(newFreezePositionY);
			ImGui::SameLine();
			if (ImGui::Checkbox("Z##FreezePosZ", &newFreezePositionZ))
				FreezePositionZ(newFreezePositionZ);

			bool newFreezeRotationX = GetFreezeRotationX();
			bool newFreezeRotationY = GetFreezeRotationY();
			bool newFreezeRotationZ = GetFreezeRotationZ();			
			ImGui::Text("Freeze rotation: ");
			if (ImGui::Checkbox("X##FreezeRotX", &newFreezeRotationX))
				FreezeRotationX(newFreezeRotationX);
			ImGui::SameLine();
			if (ImGui::Checkbox("Y##FreezeRotY", &newFreezeRotationY))
				FreezeRotationY(newFreezeRotationY);
			ImGui::SameLine();
			if (ImGui::Checkbox("Z##FreezeRotZ", &newFreezeRotationZ))
				FreezeRotationZ(newFreezeRotationZ);

			ImGui::TreePop();
		}
	}
	else
		DrawDeleteButton(owner, this);
	
	return true;
}

void C_RigidBody::UpdateRB()
{
	// Body type
	if (bodyType == "Static")
		body->setType(reactphysics3d::BodyType::STATIC);

	else if (bodyType == "Dynamic")
		body->setType(reactphysics3d::BodyType::DYNAMIC);

	else
		body->setType(reactphysics3d::BodyType::KINEMATIC);

	// Use gravity
	body->enableGravity(useGravity);

	// Mass
	body->setMass(mass);

	// Constraints
	body->setLinearLockAxisFactor(reactphysics3d::Vector3(!freezePositionX, !freezePositionY, !freezePositionZ));
	body->setAngularLockAxisFactor(reactphysics3d::Vector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));
}

void C_RigidBody::Save(Json& json) const
{
	json["type"] = "rigidBody";

	json["body_type"] = bodyType;
	json["use_gravity"] = useGravity;
	json["mass"] = mass;
	json["freeze_position_x"] = freezePositionX;
	json["freeze_position_y"] = freezePositionY;
	json["freeze_position_z"] = freezePositionZ;
	json["freeze_rotation_x"] = freezeRotationX;
	json["freeze_rotation_y"] = freezeRotationY;
	json["freeze_rotation_z"] = freezeRotationZ;
}

void C_RigidBody::Load(Json& json)
{
	bodyType = json.at("body_type");
	useGravity = json.at("use_gravity");
	mass = json.at("mass");
	freezePositionX = json.at("freeze_position_x");
	freezePositionY = json.at("freeze_position_y");
	freezePositionZ = json.at("freeze_position_z");
	freezeRotationX = json.at("freeze_rotation_x");
	freezeRotationY = json.at("freeze_rotation_y");
	freezeRotationZ = json.at("freeze_rotation_z");

	hasUpdated = true;
}

