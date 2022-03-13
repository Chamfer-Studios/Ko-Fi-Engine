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
}
void ComponentCollider2::Load(Json& json)
{
}

// On inspector draw
bool ComponentCollider2::InspectorDraw(PanelChooser* chooser)
{
	return true;
}
