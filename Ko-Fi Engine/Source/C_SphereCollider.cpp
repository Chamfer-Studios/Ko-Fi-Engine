#include "C_SphereCollider.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_RigidBody.h"

C_SphereCollider::C_SphereCollider(GameObject* parent) : Component(parent)
{
	this->type = ComponentType::SPHERE_COLLIDER;
}

C_SphereCollider::~C_SphereCollider()
{
}

bool C_SphereCollider::Start()
{
	float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
	sphereShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createSphereShape(boundingBoxSize.x/2);
	reactphysics3d::Transform transform = reactphysics3d::Transform::identity();

	collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(sphereShape, transform);
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
	if (ImGui::CollapsingHeader("Sphere Collider"))
	{
		std::string newFilter = GetFilter() == "" ? "Set Filter" : GetFilter();
		if (ImGui::BeginCombo("Set Filter##", newFilter.c_str()))
		{
			std::map<unsigned int, std::string> filterMap = owner->GetEngine()->GetPhysics()->GetFiltersMap();
			bool** filterMatrix = owner->GetEngine()->GetPhysics()->filterMatrix;
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
		float newScaleFactor = GetScaleFactor();
		if (ImGui::DragFloat("##scale", &newScaleFactor, 0.1f, 1.0f, 50000.0f))
		{
			SetScaleFactor(newScaleFactor);
			UpdateScaleFactor();
		}
	}
	return true;
}

void C_SphereCollider::Save(Json& json) const
{
	json["type"] = "sphereCollider";

	json["filter"] = filter;
	json["is_trigger"] = isTrigger;
	json["scale_factor"] = scaleFactor;
	json["center"] = { center.x, center.y, center.z };
}

void C_SphereCollider::Load(Json& json)
{
	filter = json.at("filter");
	UpdateFilter();

	isTrigger = json.at("is_trigger");
	UpdateIsTrigger();

	scaleFactor = json.at("scale_factor");
	UpdateScaleFactor();

	std::vector<float> values = json.at("center").get<std::vector<float>>();
	center = float3(values[0], values[1], values[2]);
	values.clear();
	UpdateCenter();
}

void C_SphereCollider::UpdateFilter()
{
	std::map<unsigned int, std::string> filterMap = owner->GetEngine()->GetPhysics()->GetFiltersMap();
	bool** filterMatrix = owner->GetEngine()->GetPhysics()->filterMatrix;
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

void C_SphereCollider::UpdateScaleFactor()
{
	float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
	reactphysics3d::Transform oldTransform = collider->getLocalToBodyTransform();
	owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
	owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroySphereShape(sphereShape);
	sphereShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createSphereShape((boundingBoxSize.x / 2) * scaleFactor);
	collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(sphereShape, oldTransform);
}

void C_SphereCollider::UpdateIsTrigger()
{
	collider->setIsTrigger(isTrigger);
}

void C_SphereCollider::UpdateCenter()
{
	reactphysics3d::Transform newCenterTransform(reactphysics3d::Vector3(center.x, center.y, center.z), collider->getLocalToBodyTransform().getOrientation());
	collider->setLocalToBodyTransform(newCenterTransform);
}
