#include "ComponentRigidBody.h"

ComponentRigidBody::ComponentRigidBody(GameObject* parent) : Component(parent)
{
	type = ComponentType::RIGID_BODY;

	CreateDynamic();
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

bool ComponentRigidBody::Update(float dt)
{
	bool ret = true;



	if (owner->GetEngine()->GetPhysics()->IsSimulating())
	{
		if (isKinematic)
		{

			ret = RigidBodyUpdatesTransform();

			physx::PxVec3 lVel = dynamicBody->getLinearVelocity();
			if (lVel.y > 30.0f) lVel.y = 30.0f;
			linearVel = { lVel.x, lVel.y, lVel.z };
			physx::PxVec3 aVel = dynamicBody->getAngularVelocity();
			angularVel = { aVel.x, aVel.y, aVel.z };

			// Check each frame if rigid body attributes have a pending update
			if (hasUpdated)
				UpdatePhysicsValues();
		}
		else
		{
			ret = TransformUpdatesRigidBody();
		}
	}
	else // If it's in the engine without play mode
	{
		ret = TransformUpdatesRigidBody();
	}

	return ret;
}

// Called whenever a rigid body attribute is changed
void ComponentRigidBody::UpdatePhysicsValues()
{
	if (isKinematic)
	{
		// Delete the actor in the scene, to create a newer updated version
		owner->GetEngine()->GetPhysics()->DeleteActor(dynamicBody);

		// Set mass & density
		dynamicBody->setMass(mass);
		physx::PxRigidBodyExt::updateMassAndInertia(*dynamicBody, density);

		// Set gravity & static/kinematic
		dynamicBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
		dynamicBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, !isKinematic);

		// Set velocities
		dynamicBody->setLinearVelocity(physx::PxVec3(linearVel.x, linearVel.y, linearVel.z));
		physx::PxVec3 asd = dynamicBody->getLinearVelocity();
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
	else
	{
		// Delete the actor in the scene, to create a newer updated version
		owner->GetEngine()->GetPhysics()->DeleteActor(staticBody);

		// Set gravity & static/kinematic
		staticBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);

		// Add the actor in the scene again
		owner->GetEngine()->GetPhysics()->AddActor(staticBody, owner);
	}

	hasUpdated = false;
}

// Update rigid body by the owner's transform
bool ComponentRigidBody::TransformUpdatesRigidBody()
{
	physx::PxRigidActor* body = nullptr;
	if (isKinematic)
		body = dynamicBody;
	else body = staticBody;

	if (!body)
		return false;

	owner->GetEngine()->GetPhysics()->DeleteActor(body);

	float3 pos = owner->GetTransform()->GetPosition();
	Quat rot = Quat::FromEulerXYZ(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);

	physx::PxTransform pxTransform;
	pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
	pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);
	body->setGlobalPose(pxTransform, true);

	owner->GetEngine()->GetPhysics()->AddActor(body, owner);

	return true;
}

bool ComponentRigidBody::RigidBodyUpdatesTransform()
{
	physx::PxRigidActor* body = nullptr;
	if (isKinematic)
		body = dynamicBody;
	else body = staticBody;

	if (!body)
		return false;

	physx::PxTransform transform = body->getGlobalPose();
	float3 position = float3(transform.p.x, transform.p.y, transform.p.z);
	Quat rotation = Quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
	float3 rot = rotation.ToEulerXYZ();
	float3 scale = owner->GetTransform()->GetScale();

	//float4x4 newTransform = float4x4::FromTRS(position, rotation, scale);
	//owner->GetTransform()->SetGlobalTransform(newTransform);
	owner->GetTransform()->SetPosition(position);
	owner->GetTransform()->SetRotation(rot);
	owner->GetTransform()->SetScale(scale);

	return true;
}

void ComponentRigidBody::Set2DVelocity(float2 vel)
{
	linearVel.x = vel.x;
	linearVel.z = vel.y;
	if (dynamicBody)
	{
		dynamicBody->setLinearVelocity(physx::PxVec3(linearVel.x, linearVel.y, linearVel.z));
	}
}

void ComponentRigidBody::StopMovement()
{
	if (!isKinematic)
		return;

	dynamicBody->setLinearVelocity(physx::PxVec3(0, 0, 0));
	dynamicBody->setAngularVelocity(physx::PxVec3(0, 0, 0));

	physx::PxVec3 lVel = dynamicBody->getLinearVelocity();
	linearVel = { lVel.x, lVel.y, lVel.z };
	physx::PxVec3 aVel = dynamicBody->getAngularVelocity();
	angularVel = { aVel.x, aVel.y, aVel.z };
}

// Serialization 
void ComponentRigidBody::Save(Json& json) const
{
	json["type"] = "rigidBody";

	json["is_kinematic"] = isKinematic;
	json["use_gravity"] = useGravity;

	json["mass"] = mass;
	json["density"] = density;

	json["freeze_position_x"] = freezePositionX;
	json["freeze_position_y"] = freezePositionY;
	json["freeze_position_z"] = freezePositionZ;
	json["freeze_rotation_x"] = freezeRotationX;
	json["freeze_rotation_y"] = freezeRotationY;
	json["freeze_rotation_z"] = freezeRotationZ;

	json["gravity"] = owner->GetEngine()->GetPhysics()->GetGravity();
}
void ComponentRigidBody::Load(Json& json)
{
	isKinematic = json.at("is_kinematic");
	useGravity = json.at("use_gravity");

	mass = json.at("mass");
	density = json.at("density");

	freezePositionX = json.at("freeze_position_x");
	freezePositionY = json.at("freeze_position_y");
	freezePositionZ = json.at("freeze_position_z");
	freezeRotationX = json.at("freeze_rotation_x");
	freezeRotationY = json.at("freeze_rotation_y");
	freezeRotationZ = json.at("freeze_rotation_z");

	float grav = json.at("gravity");
	owner->GetEngine()->GetPhysics()->SetGravity(grav);

	if (isKinematic)
		CreateDynamic();
	else CreateStatic();

	hasUpdated = true;
}

// On inspector draw
bool ComponentRigidBody::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (IsStatic())
	{
		if (ImGui::CollapsingHeader("Static body"))
		{
			if (ImGui::Button("Make Dynamic"))
				SetDynamic();

			// Inspector's gravity
			ImGui::Separator();
			ImGui::Text("Use gravity");
			ImGui::SameLine();
			if (ImGui::Checkbox("##gravstatic", &useGravity))
			{
				hasUpdated = true;
			}
			ImGui::Text("Scene gravity");
			ImGui::SameLine();
			float grav = owner->GetEngine()->GetPhysics()->GetGravity();
			if (ImGui::DragFloat("##gravfloatstatic", &grav, 0.1f, -10.0f, 10.0f))
			{
				owner->GetEngine()->GetPhysics()->SetGravity(grav);
			}
			if (ImGui::Button("Default gravity"))
				owner->GetEngine()->GetPhysics()->SetGravity(9.81f);
			ImGui::Separator();
		}

		return ret;
	}
	else 
	{
		if (ImGui::CollapsingHeader("Rigid body"))
		{
			if (ImGui::Button("Make Static"))
				SetStatic();
			
			// Inspector's gravity
			ImGui::Separator();
			ImGui::Text("Use gravity");
			ImGui::SameLine();
			if (ImGui::Checkbox("##gravdyn", &useGravity))
			{
				hasUpdated = true;
			}
			ImGui::Text("Scene gravity");
			ImGui::SameLine();
			float grav = owner->GetEngine()->GetPhysics()->GetGravity();
			if (ImGui::DragFloat("##gravfloatdyn", &grav, 0.1f, -10.0f, 10.0f))
			{
				owner->GetEngine()->GetPhysics()->SetGravity(grav);
			}
			if (ImGui::Button("Default gravity"))
				owner->GetEngine()->GetPhysics()->SetGravity(9.81f);
			ImGui::Separator();

			// Dynamic body attributes
			ImGui::Text("Mass");
			ImGui::SameLine();
			float newMass = mass;
			if (ImGui::DragFloat("##mass", &newMass, 0.1f, 0.0f, 20.0f))
			{
				mass = newMass;
				hasUpdated = true;
			}
			ImGui::Text("Density");
			ImGui::SameLine();
			float newDen = density;
			if (ImGui::DragFloat("##density", &newDen, 0.1f, 0.0f, 20.0f))
			{
				density = newDen;
				hasUpdated = true;
			}
			if (ImGui::CollapsingHeader("Constraints"))
			{
				ImGui::Text("Freeze position	");
				ImGui::SameLine();
				if (ImGui::Checkbox("X##X", &freezePositionX))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Y##Y", &freezePositionY))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Z##Z", &freezePositionZ))
					hasUpdated = true;
				ImGui::Text("Freeze rotation	");
				ImGui::SameLine();
				if (ImGui::Checkbox("X##X2", &freezeRotationX))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Y##Y2", &freezeRotationY))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Z##Z2", &freezeRotationZ))
					hasUpdated = true;
			}
		}
	}

	return ret;
}
// ------------------------------------------------------

void ComponentRigidBody::SetStatic()
{
	isKinematic = false;
	useGravity = false;
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
			LOG_BOTH("Could't create static body!!!\n");
			isKinematic = false;
			useGravity = true;
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
	else
	{
		LOG_BOTH("Dynamic body didn't exist when trying to SetStatic()\n");
	}
}

void ComponentRigidBody::SetDynamic()
{
	isKinematic = true;
	useGravity = true;
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
			LOG_BOTH("Could't create dynamic body!!!\n");
			isKinematic = true;
			useGravity = false;
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
	else
	{
		LOG_BOTH("Static body didn't exist when trying to SetDynamic()\n");
	}
}

void ComponentRigidBody::CreateDynamic()
{
	float3 pos = owner->GetTransform()->GetPosition();
	Quat rot = Quat::FromEulerXYZ(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);

	physx::PxTransform pxTransform;
	pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
	pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);

	// Create physx rigid dynamic body and add it to the physics actor vector
	dynamicBody = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createRigidDynamic(pxTransform);
	if (!dynamicBody)
	{
		owner->DeleteComponent(this);

		LOG_BOTH("Could not create rigid body!!\n");
		return;
	}
	/*owner->GetEngine()->GetPhysics()->AddActor(dynamicBody, owner);*/

	// We apply first physics to the actor before it is added to the actor vector
	UpdatePhysicsValues();
}

void ComponentRigidBody::CreateStatic()
{
	float3 pos = owner->GetTransform()->GetPosition();
	Quat rot = Quat::FromEulerXYZ(owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);

	physx::PxTransform pxTransform;
	pxTransform.p = physx::PxVec3(pos.x, pos.y, pos.z);
	pxTransform.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);

	// Create physx rigid dynamic body and add it to the physics actor vector
	staticBody = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createRigidStatic(pxTransform);
	if (!staticBody)
	{
		owner->DeleteComponent(this);

		LOG_BOTH("Could not create static rigid body!!\n");
		return;
	}
	/*owner->GetEngine()->GetPhysics()->AddActor(dynamicBody, owner);*/

	// We apply first physics to the actor before it is added to the actor vector
	UpdatePhysicsValues();
}
