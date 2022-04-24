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
	if (ImGui::CollapsingHeader("Capsule Collider"))
	{

		if (ImGui::BeginCombo("Set Filter##", currentFilter.c_str()))
		{
			std::map<unsigned int, std::string> filterMap = owner->GetEngine()->GetPhysics()->GetFiltersMap();
			bool** filterMatrix = owner->GetEngine()->GetPhysics()->filterMatrix;
			for (auto iter = filterMap.begin(); iter != filterMap.end(); ++iter)
			{
				if (ImGui::Selectable(iter->second.c_str()))
				{
					currentFilter = iter->second;
					collider->setCollisionCategoryBits(iter->first);
					unsigned int mask = 0;
					for (int i = 0; i < filterMap.size(); ++i)
					{
						if (filterMatrix[iter->first - 1][i])
						{
							mask |= i + 1;
						}
					}
					collider->setCollideWithMaskBits(mask);
				}
			}

			ImGui::EndCombo();
		}

		bool isTrigger = collider->getIsTrigger();
		if (ImGui::Checkbox("Is Trigger##", &isTrigger))
		{
			collider->setIsTrigger(isTrigger);
		}

		ImGui::Text("Center");
		ImGui::SameLine();
		float3 centerPoint = float3(collider->getLocalToBodyTransform().getPosition().x, collider->getLocalToBodyTransform().getPosition().y, collider->getLocalToBodyTransform().getPosition().z);
		if (ImGui::DragFloat3("##center", &(centerPoint[0]), 0.1f))
		{
			reactphysics3d::Transform newCenterTransform(reactphysics3d::Vector3(centerPoint.x, centerPoint.y, centerPoint.z), collider->getLocalToBodyTransform().getOrientation());
			collider->setLocalToBodyTransform(newCenterTransform);
		}
		ImGui::Text("Scale");
		ImGui::SameLine();
		if (ImGui::DragFloat2("##scale", &(scaleFactor[0]), 0.1f, 1.0f, 50000.0f))
		{
			float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
			reactphysics3d::Transform oldTransform = collider->getLocalToBodyTransform();
			owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
			owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroyCapsuleShape(capsuleShape);
			capsuleShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createCapsuleShape((boundingBoxSize.x / 2) * scaleFactor.x, (boundingBoxSize.y / 2) * scaleFactor.y);

			collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(capsuleShape, oldTransform);

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
