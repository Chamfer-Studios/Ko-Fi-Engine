#include "C_CapsuleCollider.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_RigidBody.h"

C_CapsuleCollider::C_CapsuleCollider(GameObject* parent) : Component(parent)
{

}

C_CapsuleCollider::~C_CapsuleCollider()
{
}

bool C_CapsuleCollider::Start()
{
	float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
	capsuleShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createCapsuleShape(boundingBoxSize.x/2, boundingBoxSize.y/2);
	reactphysics3d::Transform transform = reactphysics3d::Transform::identity();

	collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(capsuleShape, transform);
	return true;
}

bool C_CapsuleCollider::Update(float dt)
{
	return true;
}

bool C_CapsuleCollider::CleanUp()
{
	return true;
}

bool C_CapsuleCollider::InspectorDraw(PanelChooser* chooser)
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

void C_CapsuleCollider::Save(Json& json) const
{
}

void C_CapsuleCollider::Load(Json& json)
{
}
