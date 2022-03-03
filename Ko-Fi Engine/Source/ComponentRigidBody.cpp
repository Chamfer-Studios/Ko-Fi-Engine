#include "ComponentRigidBody.h"

ComponentRigidBody::ComponentRigidBody(GameObject* parent) : Component(parent)
{
	type = ComponentType::RIGID_BODY;

	float3 pos = owner->GetTransform()->GetPosition();
	Quat rot = Quat::FromEulerXYX(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);

	physx::PxTransform pxTransform;
	pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
	pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);

	// Create physx rigid dynamic body and add it to the physics actor vector
	rigidBody = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createRigidDynamic(pxTransform);
	if (!rigidBody)
	{
		delete this;

		LOG_BOTH("Could not create rigid body at ComponentRigidBody.h!!");
		return;
	}

	// We apply first physics to the actor before it is added to the actor vector
	ApplyPhysics();

	owner->GetEngine()->GetPhysics()->AddActor(rigidBody);
}

ComponentRigidBody::~ComponentRigidBody()
{
	if (rigidBody)
	{
		owner->GetEngine()->GetPhysics()->DeleteActor(rigidBody);
		rigidBody->release();
	}	
}

bool ComponentRigidBody::Start()
{
	return true;
}

bool ComponentRigidBody::Update()
{

	if (hasUpdated)
		ApplyPhysics();

	return true;
}

bool ComponentRigidBody::CleanUp()
{

	return true;
}

void ComponentRigidBody::ApplyPhysics()
{
	// This is pseudocoded, i don't know how this will work later
	if (!rigidBody)
		return;

	// Delete the actor in the scene vector
	owner->GetEngine()->GetPhysics()->DeleteActor(rigidBody);

	// Do whatever to update the position


	// Add the actor in the scene vector again
	owner->GetEngine()->GetPhysics()->AddActor(rigidBody);

	hasUpdated = false;
}

void ComponentRigidBody::SetStatic()
{
	isStatic = true;
	affectGravity = false;
	hasUpdated = true;


	ApplyPhysics();
}

void ComponentRigidBody::SetDynamic()
{
	isStatic = false;
	affectGravity = true;
	hasUpdated = true;


	ApplyPhysics();
}
