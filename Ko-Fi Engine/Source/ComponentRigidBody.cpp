#include "ComponentRigidBody.h"

ComponentRigidBody::ComponentRigidBody(GameObject *parent) : Component(parent)
{
	type = ComponentType::RIGID_BODY;

	if (!isStatic)
		CreateDynamic();
	else
		CreateStatic();
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

bool ComponentRigidBody::CleanUp()
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
	return true;
}

bool ComponentRigidBody::Update(float dt)
{
	bool ret = true;

	if (owner->GetEngine()->GetPhysics()->IsSimulating())
	{
		if (!isStatic)
		{
			ret = RigidBodyUpdatesTransform();

			physx::PxVec3 lVel = dynamicBody->getLinearVelocity();
			physx::PxVec3 aVel = dynamicBody->getAngularVelocity();
			// Speed limiters
			if (lVel.y > 30.0f)
				lVel.y = 30.0f;
			linearVel = {lVel.x, lVel.y, lVel.z};
			angularVel = {aVel.x, aVel.y, aVel.z};

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
	if (!isStatic)
	{
		// Delete the actor in the scene, to create a newer updated version
		if (dynamicBody)
			owner->GetEngine()->GetPhysics()->DeleteActor(dynamicBody);

		// Set mass & density
		physx::PxRigidBodyExt::updateMassAndInertia(*dynamicBody, density);
		dynamicBody->setMass(mass);

		// Set gravity & static/kinematic
		dynamicBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
		dynamicBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);

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
	else
	{
		// Delete the actor in the scene, to create a newer updated version
		if (staticBody)
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
	physx::PxRigidActor *body = nullptr;
	if (!isStatic)
		body = dynamicBody;
	else
		body = staticBody;

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
	physx::PxRigidActor *body = nullptr;
	if (!isStatic)
		body = dynamicBody;
	else
		body = staticBody;

	if (!body)
		return false;

	physx::PxTransform transform = body->getGlobalPose();
	float3 position = float3(transform.p.x, transform.p.y, transform.p.z);
	Quat rotation = Quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
	float3 rot = rotation.ToEulerXYZ();
	float3 scale = owner->GetTransform()->GetScale();

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
	if (isStatic)
		return;

	dynamicBody->setLinearVelocity(physx::PxVec3(0, 0, 0));
	dynamicBody->setAngularVelocity(physx::PxVec3(0, 0, 0));

	physx::PxVec3 lVel = dynamicBody->getLinearVelocity();
	linearVel = {lVel.x, lVel.y, lVel.z};
	physx::PxVec3 aVel = dynamicBody->getAngularVelocity();
	angularVel = {aVel.x, aVel.y, aVel.z};
}

// Serialization
void ComponentRigidBody::Save(Json &json) const
{
	json["type"] = "rigidBody";

	json["is_static"] = isStatic;
	json["is_kinematic"] = isKinematic;
	json["use_gravity"] = useGravity;

	json["mass"] = mass;
	json["density"] = density;

	json["linear_velocity"] = {linearVel.x, linearVel.y, linearVel.z};
	json["linear_damping"] = linearDamping;
	json["angular_velocity"] = {angularVel.x, angularVel.y, angularVel.z};
	json["angular_damping"] = angularDamping;

	json["freeze_position_x"] = freezePositionX;
	json["freeze_position_y"] = freezePositionY;
	json["freeze_position_z"] = freezePositionZ;
	json["freeze_rotation_x"] = freezeRotationX;
	json["freeze_rotation_y"] = freezeRotationY;
	json["freeze_rotation_z"] = freezeRotationZ;
}
void ComponentRigidBody::Load(Json &json)
{
	isStatic = json.at("is_static");
	isKinematic = json.at("is_kinematic");
	useGravity = json.at("use_gravity");

	mass = json.at("mass");
	density = json.at("density");

	std::vector<float> values = json.at("linear_velocity").get<std::vector<float>>();
	linearVel = float3(values[0], values[1], values[2]);
	values.clear();
	linearDamping = json.at("linear_damping");
	values = json.at("angular_velocity").get<std::vector<float>>();
	angularVel = float3(values[0], values[1], values[2]);
	values.clear();
	angularDamping = json.at("angular_damping");

	freezePositionX = json.at("freeze_position_x");
	freezePositionY = json.at("freeze_position_y");
	freezePositionZ = json.at("freeze_position_z");
	freezeRotationX = json.at("freeze_rotation_x");
	freezeRotationY = json.at("freeze_rotation_y");
	freezeRotationZ = json.at("freeze_rotation_z");

	if (!isStatic)
		CreateDynamic();
	else
		CreateStatic();

	hasUpdated = true;
}

// On inspector draw
bool ComponentRigidBody::InspectorDraw(PanelChooser *chooser)
{
	bool ret = true;

	if (isStatic)
	{
		if (ImGui::CollapsingHeader("Static body", ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			DrawDeleteButton(owner, this);

			// ---------------------------------------------------------------------------------
			if (ImGui::Button("Make Dynamic"))
				SetDynamic();
			ImGui::Separator();
			// ---------------------------------------------------------------------------------

			// ---------------------------------------------------------------------------------
			if (ImGui::TreeNodeEx("Constraints"))
			{
				ImGui::Text("Freeze position:");
				if (ImGui::Checkbox("X##X", &freezePositionX))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Y##Y", &freezePositionY))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Z##Z", &freezePositionZ))
					hasUpdated = true;
				ImGui::Text("Freeze rotation:");
				if (ImGui::Checkbox("X##X2", &freezeRotationX))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Y##Y2", &freezeRotationY))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Z##Z2", &freezeRotationZ))
					hasUpdated = true;

				ImGui::TreePop();
			}
			// ---------------------------------------------------------------------------------
		}
		else
			DrawDeleteButton(owner, this);
	}
	else
	{
		if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			DrawDeleteButton(owner, this);

			// ---------------------------------------------------------------------------------
			if (ImGui::Button("Make Static"))
				SetStatic();
			ImGui::Separator();
			// ---------------------------------------------------------------------------------

			// ---------------------------------------------------------------------------------
			ImGui::Text("Use gravity");
			ImGui::SameLine();
			bool newUseGravity = useGravity;
			if (ImGui::Checkbox("##useGravity", &newUseGravity))
				SetUseGravity(newUseGravity);

			ImGui::Text("Is Kinematic");
			ImGui::SameLine();
			bool newIsKinematic = isKinematic;
			if (ImGui::Checkbox("##isKinematic", &newIsKinematic))
				SetKinematic(newIsKinematic);

			ImGui::Text("Mass");
			ImGui::SameLine();
			float newMass = mass;
			if (ImGui::DragFloat("##mass", &newMass, 0.1f, 0.01f, 20.0f))
				SetMass(newMass);

			ImGui::Text("Density");
			ImGui::SameLine();
			float newDen = density;
			if (ImGui::DragFloat("##density", &newDen, 0.1f, 0.01f, 20.0f))
				SetDensity(newDen);
			ImGui::Separator();
			// ---------------------------------------------------------------------------------

			// ---------------------------------------------------------------------------------
			if (ImGui::TreeNodeEx("Constraints"))
			{
				ImGui::Text("Freeze position:");
				if (ImGui::Checkbox("X##X", &freezePositionX))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Y##Y", &freezePositionY))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Z##Z", &freezePositionZ))
					hasUpdated = true;
				ImGui::Text("Freeze rotation:");
				if (ImGui::Checkbox("X##X2", &freezeRotationX))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Y##Y2", &freezeRotationY))
					hasUpdated = true;
				ImGui::SameLine();
				if (ImGui::Checkbox("Z##Z2", &freezeRotationZ))
					hasUpdated = true;
				ImGui::TreePop();
			}
			ImGui::Separator();
			// ---------------------------------------------------------------------------------

			// ---------------------------------------------------------------------------------
			if (ImGui::TreeNodeEx("Velocity info (read only)"))
			{
				ImGui::Text("Speed");
				ImGui::SameLine();
				float speed = GetSpeed();
				ImGui::DragFloat("##speed", &speed, 0.1f, ImGuiInputTextFlags_ReadOnly);
				ImGui::Text("Linear Velocity");
				ImGui::SameLine();
				float3 vel2 = GetLinearVelocity();
				float vel[3] = {vel2.x, vel2.y, vel2.z};
				ImGui::InputFloat3("##linearvelinput", vel, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::Text("Linear Damping");
				ImGui::SameLine();
				float newLinDamp = GetLinearDamping();
				ImGui::DragFloat("##lineardamping", &newLinDamp, 0.1f, ImGuiInputTextFlags_ReadOnly);
				ImGui::Text("Angular Velocity");
				ImGui::SameLine();
				vel2 = GetAngularVelocity();
				float angVel[3] = {vel2.x, vel2.y, vel2.z};
				ImGui::InputFloat3("##angularvelinput", angVel, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::Text("Angular Damping");
				ImGui::SameLine();
				float newAngDamp = GetAngularDamping();
				ImGui::DragFloat("##angulardamping", &newAngDamp, 0.1f, ImGuiInputTextFlags_ReadOnly);

				ImGui::TreePop();
			}
			// ---------------------------------------------------------------------------------
		}
		else
			DrawDeleteButton(owner, this);
	}

	return ret;
}
// ------------------------------------------------------

void ComponentRigidBody::SetStatic()
{
	isStatic = true;
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
	isStatic = false;
	isKinematic = false;
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
	isStatic = false;
	hasUpdated = true;

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
}

void ComponentRigidBody::CreateStatic()
{
	isStatic = true;
	hasUpdated = true;

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
}
