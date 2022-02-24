#include "ComponentCollider.h"

ComponentCollider::ComponentCollider(GameObject* parent, ColliderType _collType) : Component(parent)
{
	collType = _collType;
	type = ComponentType::COLLIDER;
}

ComponentCollider::~ComponentCollider()
{
}

bool ComponentCollider::Start()
{
	return false;
}

bool ComponentCollider::Update()
{
	AABB aabb = owner->GetComponent<ComponentMesh>()->GetGlobalAABB();
	/*
	for all aabbs
	{
		if (aabb.Intersects(aabbs[i])) 
	}
	
	*/
	
	
	// Intersect?????
	// true --> OnCollision event

	return true;
}

bool ComponentCollider::PostUpdate(float dt)
{
	return true;
}

bool ComponentCollider::CleanUp()
{
	return false;
}

bool ComponentCollider::InspectorDraw(PanelChooser* chooser)
{
	return false;
}
