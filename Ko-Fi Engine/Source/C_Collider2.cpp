#include "C_Collider2.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_RigidBody2.h"
C_Collider2::C_Collider2(GameObject* parent) : Component(parent)
{
	
}

C_Collider2::~C_Collider2()
{
}

bool C_Collider2::Start()
{
	float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
	boxShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createBoxShape(reactphysics3d::Vector3(boundingBoxSize.x, boundingBoxSize.y, boundingBoxSize.z));
	reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
	
	collider = owner->GetComponent<C_RigidBody2>()->GetBody()->addCollider(boxShape,transform);
	return true;
}

bool C_Collider2::Update(float dt)
{
	return true;
}

bool C_Collider2::CleanUp()
{
	return true;
}

bool C_Collider2::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Collider2"))
	{

	}
	return true;
}

void C_Collider2::Save(Json& json) const
{
}

void C_Collider2::Load(Json& json)
{
}
