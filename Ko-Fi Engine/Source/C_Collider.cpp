#include "C_Collider.h"

ComponentCollider2::ComponentCollider2(GameObject* parent, ColliderType2 collType) : Component(parent)
{
	type = ComponentType::COLLIDER2;

	colliderType = collType;
}

ComponentCollider2::~ComponentCollider2()
{
	if (shape)
		shape->release();

	shape = nullptr;
}

bool ComponentCollider2::Update(float dt)
{
	return true;
}

bool ComponentCollider2::CleanUp()
{
	return true;
}

void ComponentCollider2::CreateCollider(ColliderType2 collType)
{
	// Delete attached current shape. We will create it afterwards
	if (shape)
	{
		shape->release();
		shape = nullptr;
	}

	// If a rigid body is not created, then create it
	// If it is already created, update the actor list (removing and creating again)
	if (!owner->GetComponent<ComponentRigidBody>())
		owner->CreateComponent<ComponentRigidBody>();
	else
		owner->GetEngine()->GetPhysics()->DeleteActor(owner->GetComponent<ComponentRigidBody>()->GetRigidBody());

	switch (collType)
	{
	case ColliderType2::BOX:
	{

		break;
	}
	case ColliderType2::SPHERE:
	{

		break;
	}
	case ColliderType2::CAPSULE:
	{

		break;
	}
	default:
	{

		break;
	}
	}



	owner->GetEngine()->GetPhysics()->AddActor(owner->GetComponent<ComponentRigidBody>()->GetRigidBody(), owner);
}

// Serialization 
void ComponentCollider2::Save(Json& json) const
{
	json["type"] = "collider2";

	json["collider_type"] = (int)colliderType;

	json["enabled"] = enabled;
	json["is_trigger"] = isTrigger;
}
void ComponentCollider2::Load(Json& json)
{
	SetColliderType((ColliderType2)json.at("collider_type"));

	enabled = json.at("enabled");
	isTrigger = json.at("is_trigger");
}

// On inspector draw
bool ComponentCollider2::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Component Collider (physx)"))
	{
		// Take care with the order in the combo, it has to follow the ColliderType2 enum class order
		ImGui::Combo("##combocollider", &colliderTypeInt, "Add Collider\0Box\0Sphere\0Capsule");

		ImGui::SameLine();

		if ((ImGui::Button("Assign")))
		{
			switch (colliderTypeInt)
			{
			case (int)ColliderType2::NONE: break;
			case (int)ColliderType2::BOX: SetColliderType((ColliderType2)colliderTypeInt); break;
			case (int)ColliderType2::SPHERE: SetColliderType((ColliderType2)colliderTypeInt); break;
			case (int)ColliderType2::CAPSULE: SetColliderType((ColliderType2)colliderTypeInt); break;
			}
			colliderTypeInt = 0; // This will reset the button to default when clicked (Add collider == NONE in enum)
		}

		ImGui::Text("Current collider: ");
		ImGui::SameLine();
		ImGui::Text("%s", ColliderTypeToString(colliderType));

		ImGui::Separator();

		if (ImGui::Checkbox("Enable##", &enabled));
		if (ImGui::Checkbox("IsTrigger##", &isTrigger));
	}
	return ret;
}

const char* ComponentCollider2::ColliderTypeToString(const ColliderType2 collType)
{
	switch (collType)
	{
	case ColliderType2::BOX:
		return "BOX";
	case ColliderType2::CAPSULE:
		return "CAPSULE";
	case ColliderType2::SPHERE:
		return "SPHERE";
	default:
		return nullptr;
	}
	return nullptr;
}
