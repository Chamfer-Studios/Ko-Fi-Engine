#include "ComponentCollider.h"

#include "Globals.h"
#include "ComponentMesh.h"
#include "CollisionDetector.h"

ComponentCollider::ComponentCollider(GameObject* parent, ColliderType _collType) : Component(parent)
{
	collType = _collType;
	type = ComponentType::COLLIDER;
}

ComponentCollider::~ComponentCollider()
{
	CleanUp();
}

bool ComponentCollider::Start()
{
	return true;
}

bool ComponentCollider::Update(float dt)
{
	//check if the GO with the componentCollider has an actual mesh and bounding box to collide with
	//in case not, delete component and pop an error of "no mesh"
	//TODO: Search for the childs' AABBs?
	if (owner->GetComponent<ComponentMesh>() == nullptr)
	{
		//LOG("No mesh found in GameObject %s, deleting component collider automaticaly", owner->name.c_str());
		owner->DeleteComponent(this);
	}
	else
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
	if (collType == ColliderType::ENEMY || collType == ColliderType::PLAYER)
			owner->GetEngine()->GetCollisionDetector()->RemoveCollidableEntity(owner);

	lastObjectCollided = nullptr;

	return false;
}

bool ComponentCollider::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	if (ImGui::CollapsingHeader("Component Collider", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		ImGui::Combo("###combo", &colliderType, "Collider Type\0Player\0Enemy\0Wall\0Floor");

		ImGui::SameLine();

		if ((ImGui::Button("Assign Type")))
		{
			switch (colliderType)
			{
			case (int)ColliderType::UNDEFINED: break;
			case (int)ColliderType::PLAYER: SetColliderType((ColliderType)colliderType); break;
			case (int)ColliderType::ENEMY: SetColliderType((ColliderType)colliderType); break;
			case (int)ColliderType::WALL: SetColliderType((ColliderType)colliderType); break;
			case (int)ColliderType::FLOOR: SetColliderType((ColliderType)colliderType); break;
			}
		}
		
		ImGui::Text("Currenly Colliding...");
		ImGui::SameLine();
		if ( isColliding == true )
			ImGui::Text(lastObjectCollided->GetName());
		else
			ImGui::Text("No collision");
	}             
	else
		DrawDeleteButton(owner, this);

	return ret;
}

void ComponentCollider::Save(Json& json) const
{
	json["type"] = "collider";

	json["is_trigger"] = isTrigger;
	json["collider_type"] = (int)collType;
}

void ComponentCollider::Load(Json& json)
{
	isTrigger = json.at("is_trigger");
	SetColliderType((ColliderType)json.at("collider_type"));
}

void ComponentCollider::SetColliderType(ColliderType type)
{
	collType = type;

	if (type == ColliderType::ENEMY || type == ColliderType::PLAYER)
	{
		//add owner to collidable entities list.
		owner->GetEngine()->GetCollisionDetector()->AddCollidableEntity(owner);
	}
}
