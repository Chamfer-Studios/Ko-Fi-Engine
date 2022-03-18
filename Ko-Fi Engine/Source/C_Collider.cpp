#include "C_Collider.h"

ComponentCollider2::ComponentCollider2(GameObject* parent, ColliderShape collType) : Component(parent)
{
	type = ComponentType::COLLIDER2;

	colliderShape = collType;
}

ComponentCollider2::~ComponentCollider2()
{
	if (shape)
	{
		if (owner->GetComponent<ComponentRigidBody>())
			owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->detachShape(*shape);
		shape->release();
		shape = nullptr;
	}
}

bool ComponentCollider2::Update(float dt)
{
	bool ret = true;

	if (hasUpdated)
	{
		ret = UpdateCollider();
		hasUpdated = false;
	}

	return ret;
}

// TODO: Separate updating between shape and state
bool ComponentCollider2::UpdateCollider()
{
	bool ret = true;

	owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->detachShape(*shape);

	// SHAPE UPDATE
	shape->release();
	switch (GetColliderShape())
	{
	case ColliderShape::BOX:

		break;
	case ColliderShape::SPHERE:

		break;
	case ColliderShape::CAPSULE:

		break;
	default:
		break;
	}

	// STATE UPDATE
	shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
	shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);

	physx::PxFilterData filterData;
	filterData.word0 = (int)GetCollisionLayer();

	shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->attachShape(*shape);

	return ret;
}

void ComponentCollider2::CreateCollider(ColliderShape collType)
{
	// SHAPE CREATION
	if (shape)
		shape->release();

	// If a rigid body is not created, then create it
	// If it is already created, update the actor list (removing and creating again)
	if (!owner->GetComponent<ComponentRigidBody>())
		owner->CreateComponent<ComponentRigidBody>();
	else
		owner->GetEngine()->GetPhysics()->DeleteActor(owner->GetComponent<ComponentRigidBody>()->GetRigidBody());

	switch (collType)
	{
	case ColliderShape::BOX:
		//float3 size = boxCollSize;
		//physx::PxBoxGeometry boxGeometry(size.x / 2, size.y / 2, size.z / 2);
		//shape = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createShape(boxGeometry, physx::PxMaterial(0, 0, 0));
		break;
	case ColliderShape::SPHERE:

		break;
	case ColliderShape::CAPSULE:

		break;
	default:
		break;
	}

	// STATE CREATION
	shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
	shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);

	physx::PxFilterData filterData;
	filterData.word0 = (int)GetCollisionLayer();

	shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->attachShape(*shape);

	owner->GetEngine()->GetPhysics()->AddActor(owner->GetComponent<ComponentRigidBody>()->GetRigidBody(), owner);
}

// Serialization 
void ComponentCollider2::Save(Json& json) const
{
	json["type"] = "collider2";

	json["collider_type"] = (int)colliderShape;
	json["collision_layer"] = (int)collisionLayer;

	json["enabled"] = enabled;
	json["is_trigger"] = isTrigger;
}
void ComponentCollider2::Load(Json& json)
{
	SetColliderShape((ColliderShape)json.at("collider_type"));
	SetCollisionLayer((CollisionLayer)json.at("collision_layer"));

	enabled = json.at("enabled");
	isTrigger = json.at("is_trigger");
}

// On inspector draw
bool ComponentCollider2::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Component Collider (physx)"))
	{
		// COLLIDER SHAPE -----------------------------------------------------------------------------------------------
		ImGui::Text("Collider Shape:");
		// Take care with the order in the combo, it has to follow the ColliderShape enum class order
		ImGui::Combo("##combocollidershape", &colliderShapeInt, "Add Collider\0Box\0Sphere\0Capsule");
		ImGui::SameLine();
		if ((ImGui::Button("Assign##collidershape")))
		{
			switch (colliderShapeInt)
			{
			case (int)ColliderShape::NONE: break;
			case (int)ColliderShape::BOX: SetColliderShape((ColliderShape)colliderShapeInt); break;
			case (int)ColliderShape::SPHERE: SetColliderShape((ColliderShape)colliderShapeInt); break;
			case (int)ColliderShape::CAPSULE: SetColliderShape((ColliderShape)colliderShapeInt); break;
			}
			colliderShapeInt = 0; // This will reset the button to default when clicked
		}
		ImGui::Text("Current collider shape: ");
		ImGui::SameLine();
		ImGui::Text("%s", GetColliderShapeString());

		ImGui::Separator();

		// COLLISION LAYER -----------------------------------------------------------------------------------------------
		ImGui::Text("Collison Layer:");
		// Take care with the order in the combo, it has to follow the CollisionLayer enum class order
		ImGui::Combo("##combocollisionlayer", &collisionLayerInt, "Default\0Player\0Enemy\0Bullet\0Terrain");
		ImGui::SameLine();
		if ((ImGui::Button("Assign##collisionlayer")))
		{
			switch (collisionLayerInt)
			{
			case (int)CollisionLayer::DEFAULT: SetCollisionLayer((CollisionLayer)collisionLayerInt); break;
			case (int)CollisionLayer::PLAYER: SetCollisionLayer((CollisionLayer)collisionLayerInt); break;
			case (int)CollisionLayer::ENEMY: SetCollisionLayer((CollisionLayer)collisionLayerInt); break;
			case (int)CollisionLayer::BULLET: SetCollisionLayer((CollisionLayer)collisionLayerInt); break;
			case (int)CollisionLayer::TERRAIN: SetCollisionLayer((CollisionLayer)collisionLayerInt); break;
			}
			collisionLayerInt = 0; // This will reset the button to default when clicked
		}
		ImGui::Text("Current collision layer: ");
		ImGui::SameLine();
		ImGui::Text("%s", GetCollisionLayerString());

		ImGui::Separator();

		// ATTRIBUTES -----------------------------------------------------------------------------------------------
		if (ImGui::Checkbox("Enable##", &enabled));
		if (ImGui::Checkbox("IsTrigger##", &isTrigger));
	}

	return ret;
}

// Private methods
const char* ComponentCollider2::ColliderShapeToString(const ColliderShape collShape)
{
	switch (collShape)
	{
	case ColliderShape::BOX:
		return "BOX";
	case ColliderShape::CAPSULE:
		return "CAPSULE";
	case ColliderShape::SPHERE:
		return "SPHERE";
	default:
		return "ERROR, NO COLLIDER SHAPE";
	}
	return "ERROR, NO COLLIDER SHAPE";
}
const char* ComponentCollider2::CollisionLayerToString(const CollisionLayer collLayer)
{
	switch (collLayer)
	{
	case CollisionLayer::DEFAULT:
		return "DEFAULT";
	case CollisionLayer::PLAYER:
		return "PLAYER";
	case CollisionLayer::ENEMY:
		return "ENEMY";
	case CollisionLayer::BULLET:
		return "BULLET";
	case CollisionLayer::TERRAIN:
		return "TERRAIN";
	default:
		return "ERROR, NO COLLISION LAYER";
	}
	return "ERROR, NO COLLISION LAYER";
}

