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
	dynamicBody = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createRigidDynamic(pxTransform);
	if (!dynamicBody)
	{
		owner->DeleteComponent(this);

		LOG_BOTH("Could not create rigid body!!");
		return;
	}

	// We apply first physics to the actor before it is added to the actor vector
	UpdatePhysicsValues();
}

ComponentRigidBody::~ComponentRigidBody()
{
	if (dynamicBody)
	{
		owner->GetEngine()->GetPhysics()->DeleteActor(dynamicBody);
		dynamicBody->release();
	}	
	if (staticBody)
	{
		owner->GetEngine()->GetPhysics()->DeleteActor(staticBody);
		staticBody->release();
	}
}

bool ComponentRigidBody::Update()
{
	// If it's not simulating, skip the update
	if (!owner->GetEngine()->GetPhysics()->GetInGame())
	{
		physx::PxRigidActor* stBody = nullptr;

		if (isStatic)
			stBody = staticBody;
		else stBody = dynamicBody;

		if (!stBody)
			return true;

		// Update rigid body by the GO's transform movement
		owner->GetEngine()->GetPhysics()->DeleteActor(stBody);

		float3 pos = owner->GetTransform()->GetPosition();
		Quat rot = Quat::FromEulerXYX(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);

		physx::PxTransform pxTransform;
		pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
		pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);
		stBody->setGlobalPose(pxTransform, true);

		owner->GetEngine()->GetPhysics()->AddActor(stBody, owner);

		return true;
	}

	if (!isStatic)
	{
		if (dynamicBody)
		{
			// Update GO transform by the actor's movement
			physx::PxRigidActor* dynBody = nullptr;
			dynBody = dynamicBody;

			if (!dynBody)
				return true;

			physx::PxTransform transform = dynBody->getGlobalPose();
			float3 position = float3(transform.p.x, transform.p.y, transform.p.z);
			Quat rotation = Quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
			float3 scale = owner->GetTransform()->GetScale();

			float4x4 newTransform = float4x4::FromTRS(position, rotation, scale);
			owner->GetTransform()->SetPosition(position);
			owner->GetTransform()->SetRotation(float3(rotation.x, rotation.y, rotation.z));
			owner->GetTransform()->SetScale(scale);

			// Check each frame if rigid body attributes have a pending update
			if (hasUpdated)
				UpdatePhysicsValues();

			// Set velocities
			physx::PxVec3 lVel = dynamicBody->getLinearVelocity();
			linearVel = { lVel.x, lVel.y, lVel.z };
			physx::PxVec3 aVel = dynamicBody->getAngularVelocity();
			angularVel = { aVel.x, aVel.y, aVel.z };
		}
	}
	else // is a static object
	{
		// Update rigid body by the GO's transform movement
		physx::PxRigidActor* stBody = nullptr;
		stBody = staticBody;

		if (!stBody)
			return true;

		owner->GetEngine()->GetPhysics()->DeleteActor(stBody);

		float3 pos = owner->GetTransform()->GetPosition();
		Quat rot = Quat::FromEulerXYX(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);

		physx::PxTransform pxTransform;
		pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
		pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);
		stBody->setGlobalPose(pxTransform, true);

		owner->GetEngine()->GetPhysics()->AddActor(stBody, owner);
	}

	return true;
}

// Is called whenever a dynamic body attribute is changed
void ComponentRigidBody::UpdatePhysicsValues()
{
	if (!isStatic)
	{
		// Delete the actor in the scene, to create a newer updated version
		owner->GetEngine()->GetPhysics()->DeleteActor(dynamicBody);

		// Set mass & density
		dynamicBody->setMass(mass);
		physx::PxRigidBodyExt::updateMassAndInertia(*dynamicBody, density);

		// Set gravity & static/kinematic
		dynamicBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !affectGravity);
		dynamicBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isStatic);

		// Set velocities
		dynamicBody->setLinearVelocity(physx::PxVec3(linearVel.x, linearVel.y, linearVel.z));
		dynamicBody->setAngularVelocity(physx::PxVec3(angularVel.x, angularVel.y, angularVel.z));
		dynamicBody->setLinearDamping(linearDamping);
		dynamicBody->setAngularDamping(angularDamping);

		// Set freeze positions & rotations
		dynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, freezePositionX);
		dynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, freezePositionY);
		dynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, freezePositionZ);
		dynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, freezeRotationX);
		dynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, freezeRotationY);
		dynamicBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, freezeRotationZ);

		// Add the actor in the scene again
		owner->GetEngine()->GetPhysics()->AddActor(dynamicBody, owner);

		// Wake up if the dynamic body is sleeping
		if (IsSleeping())
			dynamicBody->wakeUp();
	}

	hasUpdated = false;
}

// On inspector draw
bool ComponentRigidBody::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	if (IsStatic())
	{
		if (ImGui::CollapsingHeader("Static body", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Make Dynamic"))
				SetDynamic();
		}

		return ret;
	}

	if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Make Static"))
			SetStatic();
	}
	

	return ret;
}

void ComponentRigidBody::SetStatic()
{
	isStatic = true;
	affectGravity = false;
	hasUpdated = true;

	if (dynamicBody)
	{
		// Delete dynamic actor from scene
		owner->GetEngine()->GetPhysics()->DeleteActor(dynamicBody);

		// Create static body
		float3 pos = owner->GetTransform()->GetPosition();
		Quat rot = Quat::FromEulerXYX(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);
		physx::PxTransform pxTransform;
		pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
		pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);

		staticBody = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createRigidStatic(pxTransform);

		if (!staticBody)
		{
			LOG_BOTH("Could't create static body!!!");
			isStatic = false;
			affectGravity = true;
			return;
		}

		// Clean the dynamic body
		dynamicBody->release();
		dynamicBody = nullptr;

		// This should be modificable in the inspector, now only if is static
		freezePositionX = true;
		freezePositionY = true;
		freezePositionZ = true;
		freezeRotationX = true;
		freezeRotationY = true;
		freezeRotationZ = true;

		// Add static actor to scene
		owner->GetEngine()->GetPhysics()->AddActor(staticBody, owner);
	}
}

void ComponentRigidBody::SetDynamic()
{
	isStatic = false;
	affectGravity = true;
	hasUpdated = true;

	if (staticBody)
	{
		// Delete static actor from scene
		owner->GetEngine()->GetPhysics()->DeleteActor(staticBody);

		// Create dynamic object
		float3 pos = owner->GetTransform()->GetPosition();
		Quat rot = Quat::FromEulerXYX(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);
		physx::PxTransform pxTransform;
		pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
		pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);

		dynamicBody = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createRigidDynamic(pxTransform);

		if (!dynamicBody)
		{
			LOG_BOTH("Could't create dynamic body!!!");
			isStatic = true;
			affectGravity = false;
			return;
		}

		// Clean the static body
		staticBody->release();
		staticBody = nullptr;

		// This should be modificable in the inspector, now only if is static
		freezePositionX = false;
		freezePositionY = false;
		freezePositionZ = false;
		freezeRotationX = false;
		freezeRotationY = false;
		freezeRotationZ = false;

		// Add dynamic actor to scene
		owner->GetEngine()->GetPhysics()->AddActor(dynamicBody, owner);
	}
}
