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

		LOG_BOTH("Could not create rigid body!!");
		return;
	}

	// We apply first physics to the actor before it is added to the actor vector
	UpdatePhysicsValues();

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
	// If it's not simulating, skip this update
	/*
	If simulating(play button)
		return true
	*/

	if (rigidBody)
	{
		// Update rigid body transform by game object world transform
		owner->GetEngine()->GetPhysics()->DeleteActor(rigidBody);

		float3 newPos = owner->GetTransform()->GetPosition();
		Quat newRot = Quat::FromEulerXYX(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);

		physx::PxTransform pxTransform;
		pxTransform.p = physx::PxVec3(newPos.x, newPos.y, newPos.z);
		pxTransform.q = physx::PxQuat(newRot.x, newRot.y, newRot.z, newRot.w);

		rigidBody->setGlobalPose(pxTransform, true);

		owner->GetEngine()->GetPhysics()->AddActor(rigidBody);

		// Check each frame if rigid body attributes have a pending update
		if (hasUpdated)
			UpdatePhysicsValues();
	}

	return true;
}

bool ComponentRigidBody::CleanUp()
{

	return true;
}

// Is called whenever a rigid body attribute is changed
void ComponentRigidBody::UpdatePhysicsValues()
{
	// This is pseudocoded, i don't know how this will work later
	if (!rigidBody)
		return;

	// Delete the actor in the scene vector , to create a newer updated version
	owner->GetEngine()->GetPhysics()->DeleteActor(rigidBody);

	// Wake up if the rigid body is sleeping
	if (IsSleeping())
		rigidBody->wakeUp();

	// Set mass & density
	rigidBody->setMass(mass);
	physx::PxRigidBodyExt::updateMassAndInertia(*rigidBody, density);

	// Set gravity & static/kinematic
	rigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !affectGravity);
	rigidBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, !isStatic);

	// Set velocities
	rigidBody->setLinearVelocity(physx::PxVec3(linearVel.x, linearVel.y, linearVel.z));
	rigidBody->setAngularVelocity(physx::PxVec3(angularVel.x, angularVel.y, angularVel.z));
	rigidBody->setLinearDamping(linearDamping);
	rigidBody->setAngularDamping(angularDamping);

	// Set freeze positions & rotations
	rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, freezePositionX);
	rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, freezePositionY);
	rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, freezePositionZ);
	rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, freezeRotationX);
	rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, freezeRotationY);
	rigidBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, freezeRotationZ);

	// Add the actor in the scene vector again
	owner->GetEngine()->GetPhysics()->AddActor(rigidBody);

	hasUpdated = false;
}

// On inspector draw
bool ComponentRigidBody::InspectorDraw(PanelChooser* chooser)
{

	return true;
}

void ComponentRigidBody::SetStatic()
{
	isStatic = true;
	affectGravity = false;
	hasUpdated = true;

	freezePositionX = true;
	freezePositionY = true;
	freezePositionZ = true;
	freezeRotationX = true;
	freezeRotationY = true;
	freezeRotationZ = true;

	// We may consider setting a very huge mass and density, so forces does not move if a rigid body is static

	UpdatePhysicsValues();
}

void ComponentRigidBody::SetDynamic()
{
	isStatic = false;
	affectGravity = true;
	hasUpdated = true;

	freezePositionX = false;
	freezePositionY = false;
	freezePositionZ = false;
	freezeRotationX = false;
	freezeRotationY = false;
	freezeRotationZ = false;

	// Change mass and density to default if we changed it in SetStatic() function

	UpdatePhysicsValues();
}
