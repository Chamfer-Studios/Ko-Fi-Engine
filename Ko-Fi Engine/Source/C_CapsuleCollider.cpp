#include "C_CapsuleCollider.h"
#include "Engine.h"
#include "Globals.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_RigidBody.h"
#include "C_Mesh.h"

C_CapsuleCollider::C_CapsuleCollider(GameObject* parent) : Component(parent)
{
	type = ComponentType::CAPSULE_COLLIDER;
}

C_CapsuleCollider::~C_CapsuleCollider()
{
	CleanUp();
}

bool C_CapsuleCollider::Start()
{
	if (owner->GetComponent<C_Mesh>())
	{
		float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
		if (boundingBoxSize.y == 0)
			boundingBoxSize.y = 0.01;
		else if (boundingBoxSize.x == 0)
			boundingBoxSize.x = 0.01;
		else if (boundingBoxSize.z == 0)
			boundingBoxSize.z = 0.01;
		capsuleShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createCapsuleShape(boundingBoxSize.x < boundingBoxSize.z ? boundingBoxSize.z / 2 : boundingBoxSize.x / 2, boundingBoxSize.y / 2);
		reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(capsuleShape, transform);
	}
	else
	{
		float3 boundingBoxSize = float3(5, 5, 5);
		capsuleShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createCapsuleShape(boundingBoxSize.x < boundingBoxSize.z ? boundingBoxSize.z / 2 : boundingBoxSize.x / 2, boundingBoxSize.y / 2);
		reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(capsuleShape, transform);
	}
	
	return true;
}

bool C_CapsuleCollider::Update(float dt)
{
	if (!owner->GetComponent<C_RigidBody>())
	{
		owner->DeleteComponent(this);
	}
	return true;
}

bool C_CapsuleCollider::CleanUp()
{
	if (collider)
	{
		if (owner->GetComponent<C_RigidBody>() && owner->GetComponent<C_RigidBody>()->GetBody())
			owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
	}
	if (capsuleShape)
		owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroyCapsuleShape(capsuleShape);

	return true;
}

bool C_CapsuleCollider::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Capsule Collider", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		if (DrawDeleteButton(owner, this))
			return true;

		std::string newFilter = GetFilter();
		if (ImGui::BeginCombo("Set Filter##", newFilter == "" ? "Set Filter" : newFilter.c_str()))
		{
			std::map<unsigned int, std::string> filterMap = owner->GetEngine()->GetPhysics()->GetFiltersMap();
			for (auto iter = filterMap.begin(); iter != filterMap.end(); ++iter)
			{
				if (ImGui::Selectable(iter->second.c_str()))
				{
					SetFilter(iter->second);
					UpdateFilter();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text("Current filter:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", filter.c_str());

		bool newIsTrigger = GetIsTrigger();
		if (ImGui::Checkbox("Is Trigger##", &newIsTrigger))
		{
			SetIsTrigger(newIsTrigger);
			UpdateIsTrigger();
		}

		ImGui::Text("Center");
		ImGui::SameLine();
		float3 newCenter = GetCenter();
		if (ImGui::DragFloat3("##center", &(newCenter[0]), 0.1f))
		{
			SetCenter(newCenter);
			UpdateCenter();
		}

		ImGui::Text("Scale");
		ImGui::SameLine();
		float2 newScaleFactor = GetScaleFactor();
		if (ImGui::DragFloat2("##scale", &(newScaleFactor[0]), 0.1f, 1.0f, 50000.0f))
		{
			SetScaleFactor(newScaleFactor);
			UpdateScaleFactor();
		}
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}

void C_CapsuleCollider::Save(Json& json) const
{
	json["type"] = "capsuleCollider";
	json["filter"] = filter;
	json["is_trigger"] = isTrigger;
	json["scale_factor"] = { scaleFactor.x, scaleFactor.y};
	json["center"] = { center.x, center.y, center.z };
}

void C_CapsuleCollider::Load(Json& json)
{
	filter = json.at("filter");
	UpdateFilter();

	isTrigger = json.at("is_trigger");
	UpdateIsTrigger();

	std::vector<float> values = json.at("scale_factor").get<std::vector<float>>();
	scaleFactor = float2(values[0], values[1]);
	values.clear();
	UpdateScaleFactor();

	values = json.at("center").get<std::vector<float>>();
	center = float3(values[0], values[1], values[2]);
	values.clear();
	UpdateCenter();
}

void C_CapsuleCollider::UpdateFilter()
{
	std::map<unsigned int, std::string> filterMap = owner->GetEngine()->GetPhysics()->GetFiltersMap();
	bool** filterMatrix = owner->GetEngine()->GetPhysics()->GetFilterMatrix();
	for (auto iter : filterMap)
	{
		if (iter.second == filter)
		{
			collider->setCollisionCategoryBits(iter.first);
			unsigned int mask = 0;
			for (int i = 0; i < filterMap.size(); ++i)
			{
				if (filterMatrix[iter.first - 1][i])
				{
					mask |= i + 1;
				}
			}
			collider->setCollideWithMaskBits(mask);
		}
	}
}

void C_CapsuleCollider::UpdateScaleFactor()
{
	if (owner->GetComponent<C_Mesh>())
	{
		float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
		reactphysics3d::Transform oldTransform = collider->getLocalToBodyTransform();
		owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
		owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroyCapsuleShape(capsuleShape);
		capsuleShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createCapsuleShape((boundingBoxSize.x < boundingBoxSize.z ? boundingBoxSize.z : boundingBoxSize.x) / 2 * scaleFactor.x, (boundingBoxSize.y / 2) * scaleFactor.y);
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(capsuleShape, oldTransform);
	}
	else
	{
		float3 boundingBoxSize = float3(5, 5, 5);
		reactphysics3d::Transform oldTransform = collider->getLocalToBodyTransform();
		owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
		owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroyCapsuleShape(capsuleShape);
		capsuleShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createCapsuleShape((boundingBoxSize.x < boundingBoxSize.z ? boundingBoxSize.z : boundingBoxSize.x) / 2 * scaleFactor.x, (boundingBoxSize.y / 2) * scaleFactor.y);
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(capsuleShape, oldTransform);
	}

	UpdateFilter();

	UpdateIsTrigger();

	UpdateCenter();
}

void C_CapsuleCollider::UpdateIsTrigger()
{
	collider->setIsTrigger(isTrigger);
}

void C_CapsuleCollider::UpdateCenter()
{
	reactphysics3d::Transform newCenterTransform(reactphysics3d::Vector3(center.x, center.y, center.z), collider->getLocalToBodyTransform().getOrientation());
	collider->setLocalToBodyTransform(newCenterTransform);
}
