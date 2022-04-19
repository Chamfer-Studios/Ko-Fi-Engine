#include "C_Collider.h"

#include "GameObject.h"

#include "C_RigidBody.h"
#include "C_Mesh.h"

#include "M_Physics.h"
#include "PxPhysicsAPI.h"

#include "optick.h"

C_Collider::C_Collider(GameObject *parent, ColliderShape collType) : Component(parent)
{
	type = ComponentType::COLLIDER;
	colliderShape = collType;
	setFromAABB = true;
	CreateCollider(colliderShape);
}

C_Collider::~C_Collider()
{
	if (shape)
	{
		/*if (owner->GetComponent<C_RigidBody>())
			owner->GetComponent<C_RigidBody>()->GetRigidBody()->detachShape(*shape);*/
		shape->release();
		shape = nullptr;
	}
}

bool C_Collider::Update(float dt)
{
	bool ret = true;

	if (hasUpdated)
	{
		ret = UpdateCollider();
		hasUpdated = false;
	}

	return ret;
}

bool C_Collider::PostUpdate(float dt)
{
	bool ret = true;

	return ret;
}

bool C_Collider::UpdateCollider()
{
	bool ret = true;

	CreateCollider(colliderShape);

	return ret;
}

void C_Collider::CreateCollider(ColliderShape collType)
{
	switch (collType)
	{
	case ColliderShape::BOX:
		CreateBoxCollider();
		break;
	case ColliderShape::SPHERE:

		break;
	case ColliderShape::CAPSULE:

		break;
	case ColliderShape::NONE:

		break;
	default:

		break;
	}
}

void C_Collider::CreateBoxCollider()
{
	if (shape)
		debugFilter = (std::string *)shape->getSimulationFilterData().word0;

	// SHAPE CREATION
	if (shape)
		shape->release();
	if (shape)
		owner->GetComponent<C_RigidBody>()->GetRigidBody()->detachShape(*shape);

	owner->GetEngine()->GetPhysics()->DeleteActor(owner->GetComponent<C_RigidBody>()->GetRigidBody());

	C_Transform* currentTransform = owner->GetComponent<C_Transform>();
	float3 pos, scale;
	Quat quat;
	currentTransform->GetGlobalTransform().Decompose(pos, quat, scale);

	prevTransform = currentTransform;

	if (setFromAABB)
	{
		boxCollSize = owner->GetComponent<C_Mesh>()->GetGlobalAABB().maxPoint - owner->GetComponent<C_Mesh>()->GetGlobalAABB().minPoint;
		setFromAABB = false;
	}
	
	physx::PxBoxGeometry boxGeometry(boxCollSize.x, boxCollSize.y, boxCollSize.z);
	
	shape = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createShape(boxGeometry, *owner->GetEngine()->GetPhysics()->GetPxMaterial());

	physx::PxTransform localPose;
	float3 center = owner->GetComponent<C_Mesh>()->GetGlobalAABB().CenterPoint();
	localPose.p = physx::PxVec3(center.x, center.y, center.z);
	localPose.q = physx::PxQuat(quat.y, quat.x, quat.z, quat.w);
	shape->setLocalPose(localPose);

	// STATE CREATION
	if (shape)
	{
		shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
		shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);

		physx::PxFilterData filterData;
		filterData.word0 = (int)GetFilter();

		shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setSimulationFilterData(filterData);
		shape->setQueryFilterData(filterData);

		owner->GetComponent<C_RigidBody>()->GetRigidBody()->attachShape(*shape);
	}

	owner->GetEngine()->GetPhysics()->AddActor(owner->GetComponent<C_RigidBody>()->GetRigidBody(), owner);
}

void C_Collider::DrawCollider()
{
	if (drawCollider)
	{
		if (colliderShape == ColliderShape::BOX)
		{
			DrawBoxCollider();
		}
	}
}

void C_Collider::DrawBoxCollider()
{
	if (shape)
	{
		float3 transformOffset = owner->GetComponent<C_Transform>()->GetPosition();
		physx::PxTransform localPose;
		physx::PxVec3 center;

		localPose = shape->getLocalPose();
		center = localPose.p;

		physx::PxBoxGeometry boxGeometry;
		this->shape->getBoxGeometry(boxGeometry);

		float3 min = float3(center.x, center.y, center.z) - float3(boxGeometry.halfExtents.x/2, boxGeometry.halfExtents.y/2, boxGeometry.halfExtents.z/2);
		float3 max = float3(center.x, center.y, center.z) + float3(boxGeometry.halfExtents.x/2, boxGeometry.halfExtents.y/2, boxGeometry.halfExtents.z/2);

		glLineWidth(4.0f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);

		// Bottom 1
		glVertex3f(min.x, min.y, min.z);
		glVertex3f(max.x, min.y, min.z);

		glVertex3f(min.x, min.y, min.z);
		glVertex3f(min.x, max.y, min.z);

		glVertex3f(min.x, min.y, min.z);
		glVertex3f(min.x, min.y, max.z);

		// Bottom 2
		glVertex3f(max.x, min.y, max.z);
		glVertex3f(min.x, min.y, max.z);

		glVertex3f(max.x, min.y, max.z);
		glVertex3f(max.x, max.y, max.z);

		glVertex3f(max.x, min.y, max.z);
		glVertex3f(max.x, min.y, min.z);

		// Top 1
		glVertex3f(max.x, max.y, min.z);
		glVertex3f(min.x, max.y, min.z);

		glVertex3f(max.x, max.y, min.z);
		glVertex3f(max.x, min.y, min.z);

		glVertex3f(max.x, max.y, min.z);
		glVertex3f(max.x, max.y, max.z);

		// Top 2
		glVertex3f(min.x, max.y, max.z);
		glVertex3f(max.x, max.y, max.z);

		glVertex3f(min.x, max.y, max.z);
		glVertex3f(min.x, min.y, max.z);

		glVertex3f(min.x, max.y, max.z);
		glVertex3f(min.x, max.y, min.z);

		glEnd();
		glColor3f(1.f, 1.f, 1.f);
		glLineWidth(1.0f);
	}
	
}

// Serialization
void C_Collider::Save(Json &json) const
{
	json["type"] = "collider";

	json["collider_type"] = (int)colliderShape;
	json["filter"] = filter;

	json["enabled"] = enabled;
	json["is_trigger"] = isTrigger;
	json["draw_collider"] = drawCollider;

	json["box_collider_size"] = {boxCollSize.x, boxCollSize.y, boxCollSize.z};
	json["offset"] = {offset.x, offset.y, offset.z};
}

void C_Collider::Load(Json &json)
{
	if (json.contains("collider_type"))
		colliderShape = (ColliderShape)json.at("collider_type");
	if (json.contains("filter"))
		filter = json.at("filter");

	if (json.contains("enabled"))
		enabled = json.at("enabled");
	if (json.contains("is_trigger"))
		isTrigger = json.at("is_trigger");
	if (json.contains("draw_collider"))
		drawCollider = json.at("draw_collider");

	if (json.contains("box_collider_size"))
	{
		std::vector<float> values = json.at("box_collider_size").get<std::vector<float>>();
		boxCollSize = float3(values[0], values[1], values[2]);
		values.clear();
	}
	if (json.contains("offset"))
	{
		std::vector<float> values = json.at("offset").get<std::vector<float>>();
		offset = float3(values[0], values[1], values[2]);
		values.clear();
	}

	setFromAABB = true;
	hasUpdated = true;
}

// On inspector draw
bool C_Collider::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Component Collider (physx)", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		// COLLIDER SHAPE -----------------------------------------------------------------------------------------------
		ImGui::Text("Collider Shape:");
		// Take care with the order in the combo, it has to follow the ColliderShape enum class order
		ImGui::Combo("##combocollidershape", &colliderShapeInt, "Add Collider\0Box\0Sphere\0Capsule");
		ImGui::SameLine();
		if ((ImGui::Button("Assign##collidershape")))
		{
			SetColliderShape((ColliderShape)colliderShapeInt);
			colliderShapeInt = 0; // This will reset the button to default when clicked
			hasUpdated = true;
		}
		ImGui::Text("Current collider shape: ");
		ImGui::SameLine();
		ImGui::Text("%s", GetColliderShapeString());

		ImGui::Separator();

		if (colliderShape == ColliderShape::BOX)
		{
			// FILTERS -----------------------------------------------------------------------------------------------
			ImGui::Text("Filter:");
			if (ImGui::BeginCombo("Filter", filter.c_str()))
			{
				const std::vector<std::string> filters = owner->GetEngine()->GetPhysics()->GetFilters();

				for (int i = 0; i < filters.size(); ++i)
				{
					if (ImGui::Selectable(filters[i].c_str()))
						SetFilter(filters[i]);
				}
				ImGui::EndCombo();
			}
			ImGui::Text("Current filter: ");
			ImGui::SameLine();
			const std::string* str = GetFilter();
			ImGui::Text("%s", str->c_str());
			ImGui::Separator();

			// ATTRIBUTES -----------------------------------------------------------------------------------------------
			if (ImGui::Checkbox("Enable##", &enabled)) hasUpdated = true;
			if (ImGui::Checkbox("IsTrigger##", &isTrigger)) hasUpdated = true;
			if (ImGui::Checkbox("Draw Collider##", &drawCollider)) hasUpdated = true;
			ImGui::Separator();
			// -----------------------------------------------------------------------------------------------------------
			if ((ImGui::Button("Update size from AABB##")))
			{
				setFromAABB = true;
				hasUpdated = true;
			}

			ImGui::Text("Size:");
			float newSize[3] = { boxCollSize.x, boxCollSize.y, boxCollSize.z };
			if (ImGui::InputFloat3("##boxcollsize", newSize, "%.3f", ImGuiInputTextFlags_ReadOnly))
			{
				boxCollSize = { newSize[0], newSize[1], newSize[2] };
				hasUpdated = true;
			}

			ImGui::Text("Offset");
			float newOffset[3] = { offset.x, offset.y, offset.z };
			if (ImGui::InputFloat3("##offset", newOffset, "%.3f", ImGuiInputTextFlags_ReadOnly))
			{
				offset = { newOffset[0], newOffset[1], newOffset[2] };
				hasUpdated = true;
			}
		}
		else if (colliderShape == ColliderShape::NONE)
		{
			ImGui::Text("You have to set a collider shape first!");
		}
		else
		{
			ImGui::Text("Collider shape not supported yet!");
		}
	}
	else
		DrawDeleteButton(owner, this);

	return ret;
}

// Private methods
const char* C_Collider::ColliderShapeToString(const ColliderShape collShape)
{
	switch (collShape)
	{
	case ColliderShape::NONE:
		return "NONE";
	case ColliderShape::BOX:
		return "BOX";
	case ColliderShape::CAPSULE:
		return "CAPSULE";
	case ColliderShape::SPHERE:
		return "SPHERE";
	default:
		return "ERROR, goto C_Collider.cpp refer 'ColliderShapeToString'";
	}
	return "ERROR, NO COLLIDER SHAPE";
}
const char *C_Collider::CollisionLayerToString(const CollisionLayer collLayer)
{
	switch (collLayer)
	{
	case CollisionLayer::DEFAULT:
		return "DEFAULT";
	case CollisionLayer::PLAYER:
		return "PLAYER";
	case CollisionLayer::ENEMY:
		return "ENEMY";
	case CollisionLayer::PROJECTILE:
		return "PROJECTILE";
	case CollisionLayer::TERRAIN:
		return "TERRAIN";
	default:
		return "ERROR, goto C_Collider.cpp refer 'CollisionLayerToString'";
	}
	return "ERROR, NO COLLISION LAYER";
}
