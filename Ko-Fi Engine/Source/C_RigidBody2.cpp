#include "C_RigidBody2.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "M_SceneManager.h"
#include "C_Transform.h"


C_RigidBody2::C_RigidBody2(GameObject* parent) : Component(parent)
{

}

C_RigidBody2::~C_RigidBody2()
{
	CleanUp();
}

bool C_RigidBody2::Start()
{
	float3 gameObjectPosition = owner->GetTransform()->GetPosition();
	Quat gameObjectOrientation = owner->GetTransform()->GetRotationQuat();
	reactphysics3d::Vector3 rbPosition(gameObjectPosition.x, gameObjectPosition.y, gameObjectPosition.z);
	reactphysics3d::Quaternion rbOrientation(gameObjectOrientation.x, gameObjectOrientation.y, gameObjectOrientation.z, gameObjectOrientation.w);
	reactphysics3d::Transform rbTransform(rbPosition, rbOrientation);
	body = owner->GetEngine()->GetPhysics()->GetWorld()->createRigidBody(rbTransform);
	return true;
}

bool C_RigidBody2::Update(float dt)
{

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

bool C_RigidBody2::CleanUp()
{
	// Destroy a rigid body 
	owner->GetEngine()->GetPhysics()->GetWorld()->destroyRigidBody(body);
	return true;
}

bool C_RigidBody2::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("RigidBody2"))
	{
		if (ImGui::BeginCombo("Set Type##", bodyType.c_str()))
		{
			if (ImGui::Selectable("Static"))
			{
				body->setType(reactphysics3d::BodyType::STATIC);
				bodyType = "Static";
			}
			if (ImGui::Selectable("Dynamic"))
			{
				body->setType(reactphysics3d::BodyType::DYNAMIC);
				bodyType = "Dynamic";

			}
			if (ImGui::Selectable("Kinematic"))
			{
				body->setType(reactphysics3d::BodyType::KINEMATIC);
				bodyType = "Kinematic";

			}
			ImGui::EndCombo();
		}

		bool useGravity = body->isGravityEnabled();
		if (ImGui::Checkbox("Use Gravity##", &useGravity))
		{
			body->enableGravity(useGravity);
		}
		ImGui::Text("Mass");
		ImGui::SameLine();
		float newMass = body->getMass();
		if (ImGui::DragFloat("##mass", &newMass, 0.1f, 0.01f, 20.0f))
		{
			body->setMass(newMass);
		}

		if (ImGui::TreeNodeEx("Constraints"))
		{
			ImGui::Text("Freeze position: ");

			bool freezePositionX = !body->getLinearLockAxisFactor().x;
			bool freezePositionY = !body->getLinearLockAxisFactor().y;
			bool freezePositionZ = !body->getLinearLockAxisFactor().z;
			ImGui::SameLine();
			if (ImGui::Checkbox("X##FreezePosX", &freezePositionX))
			{
				body->setLinearLockAxisFactor(reactphysics3d::Vector3(!freezePositionX, !freezePositionY, !freezePositionZ));

			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Y##FreezePosY", &freezePositionY))
			{

				body->setLinearLockAxisFactor(reactphysics3d::Vector3(!freezePositionX, !freezePositionY, !freezePositionZ));
			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Z##FreezePosZ", &freezePositionZ))
			{
				body->setLinearLockAxisFactor(reactphysics3d::Vector3(!freezePositionX, !freezePositionY, !freezePositionZ));
			}
			ImGui::Text("Freeze rotation: ");

			bool freezeRotationX = !body->getAngularLockAxisFactor().x;
			bool freezeRotationY = !body->getAngularLockAxisFactor().y;
			bool freezeRotationZ = !body->getAngularLockAxisFactor().z;
			ImGui::SameLine();
			if (ImGui::Checkbox("X##FreezeRotX", &freezeRotationX))
			{
				body->setAngularLockAxisFactor(reactphysics3d::Vector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));

			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Y##FreezeRotY", &freezeRotationY))
			{

				body->setAngularLockAxisFactor(reactphysics3d::Vector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));
			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Z##FreezeRotZ", &freezeRotationZ))
			{
				body->setAngularLockAxisFactor(reactphysics3d::Vector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));
			}
			ImGui::TreePop();
		}
	}
	return true;
}

void C_RigidBody2::Save(Json& json) const
{
}

void C_RigidBody2::Load(Json& json)
{
}
