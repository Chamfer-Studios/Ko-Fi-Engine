#include "C_RigidBody2.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_Transform.h"


C_RigidBody2::C_RigidBody2(GameObject* parent) : Component(parent)
{

}

C_RigidBody2::~C_RigidBody2()
{
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
	const reactphysics3d::Transform& transform = body->getTransform();
	const reactphysics3d::Vector3& position = transform.getPosition();
	const reactphysics3d::Quaternion& orientation = transform.getOrientation();

	owner->GetTransform()->SetPosition(float3(position.x, position.y, position.z));
	owner->GetTransform()->SetRotationQuat(Quat(orientation.x, orientation.y, orientation.z, orientation.w));

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
		if (ImGui::Button("setStatic"))
		{
			body->setType(reactphysics3d::BodyType::STATIC);
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
