#include "C_SphereCollider.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_RigidBody2.h"

C_SphereCollider::C_SphereCollider(GameObject* parent) : Component(parent)
{

}

C_SphereCollider::~C_SphereCollider()
{
}

bool C_SphereCollider::Start()
{
	float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
	sphereShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createSphereShape(boundingBoxSize.x/2);
	reactphysics3d::Transform transform = reactphysics3d::Transform::identity();

	collider = owner->GetComponent<C_RigidBody2>()->GetBody()->addCollider(sphereShape, transform);
	return true;
}

bool C_SphereCollider::Update(float dt)
{
	return true;
}

bool C_SphereCollider::CleanUp()
{
	return true;
}

bool C_SphereCollider::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Collider2"))
	{
		bool isTrigger = collider->getIsTrigger();
		if (ImGui::Checkbox("Is Trigger##", &isTrigger))
		{
			collider->setIsTrigger(isTrigger);
		}
	}
	return true;
}

void C_SphereCollider::Save(Json& json) const
{
}

void C_SphereCollider::Load(Json& json)
{
}
