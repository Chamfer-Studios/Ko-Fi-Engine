#include "C_SphereCollider.h"
#include "Engine.h"
#include "Globals.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_RigidBody.h"
#include "C_Mesh.h"

C_SphereCollider::C_SphereCollider(GameObject* parent) : Component(parent)
{
	this->type = ComponentType::SPHERE_COLLIDER;
}

C_SphereCollider::~C_SphereCollider()
{
	CleanUp();
}

bool C_SphereCollider::Start()
{
	if (owner->GetComponent<C_Mesh>())
	{
		float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
		sphereShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createSphereShape(boundingBoxSize.MaxElement()/2);
		reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(sphereShape, transform);
	}
	else
	{
		float3 boundingBoxSize = float3(5, 5, 5);
		sphereShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createSphereShape(boundingBoxSize.MaxElement() / 2);
		reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(sphereShape, transform);
	}
	
	return true;
}

bool C_SphereCollider::Update(float dt)
{
	if (!owner->GetComponent<C_RigidBody>())
	{
		owner->DeleteComponent(this);
	}
	return true;
}

bool C_SphereCollider::CleanUp()
{
	if (collider)
	{
		if (owner->GetComponent<C_RigidBody>() && owner->GetComponent<C_RigidBody>()->GetBody())
			owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
	}
	if (sphereShape)
		owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroySphereShape(sphereShape);

	return true;
}

bool C_SphereCollider::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

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
		float newScaleFactor = GetScaleFactor();
		if (ImGui::DragFloat("##scale", &newScaleFactor, 0.1f, 1.0f, 50000.0f))
		{
			SetScaleFactor(newScaleFactor);
			UpdateScaleFactor();
		}
	}
	else
	{
		DrawDeleteButton(owner, this);
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

void C_SphereCollider::UpdateScaleFactor()
{
	if (owner->GetComponent<C_Mesh>())
	{
		float3 boundingBoxSize = owner->BoundingAABB().maxPoint - owner->BoundingAABB().minPoint;
		reactphysics3d::Transform oldTransform = collider->getLocalToBodyTransform();
		owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
		owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroySphereShape(sphereShape);
		sphereShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createSphereShape(boundingBoxSize.MaxElement() / 2 * scaleFactor);
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(sphereShape, oldTransform);
	}
	else
	{
		float3 boundingBoxSize = float3(5, 5, 5);
		reactphysics3d::Transform oldTransform = collider->getLocalToBodyTransform();
		owner->GetComponent<C_RigidBody>()->GetBody()->removeCollider(collider);
		owner->GetEngine()->GetPhysics()->GetPhysicsCommon().destroySphereShape(sphereShape);
		sphereShape = owner->GetEngine()->GetPhysics()->GetPhysicsCommon().createSphereShape(boundingBoxSize.MaxElement() / 2 * scaleFactor);
		collider = owner->GetComponent<C_RigidBody>()->GetBody()->addCollider(sphereShape, oldTransform);
	}

	UpdateFilter();

	UpdateIsTrigger();

	UpdateCenter();
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
