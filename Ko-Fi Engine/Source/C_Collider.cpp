#include "C_Collider.h"

#include "GameObject.h"

#include "ComponentRigidBody.h"
#include "ComponentMesh.h"

#include "Physics.h"
#include "PxPhysicsAPI.h"

ComponentCollider2::ComponentCollider2(GameObject *parent, ColliderShape collType) : Component(parent)
{
	type = ComponentType::COLLIDER2;
	colliderShape = collType;

	CreateCollider(colliderShape);
}

ComponentCollider2::~ComponentCollider2()
{
	CleanUp();
}

bool ComponentCollider2::CleanUp()
{
	if (shape)
	{
		/*if (owner->GetComponent<ComponentRigidBody>())
			owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->detachShape(*shape);*/
		shape->release();
		shape = nullptr;
	}
	return true;
}

bool ComponentCollider2::Update(float dt)
{
	bool ret = true;

	if (hasUpdated)
	{
		ret = UpdateCollider();
		hasUpdated = false;
	}

	return ret;
}

bool ComponentCollider2::PostUpdate(float dt)
{
	bool ret = true;

	return ret;
}

bool ComponentCollider2::UpdateCollider()
{
	bool ret = true;


	CreateCollider(colliderShape);

	return ret;
}

void ComponentCollider2::CreateCollider(ColliderShape collType)
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

void ComponentCollider2::CreateBoxCollider()
{
	if (shape)
		debugFilter = (std::string *)shape->getSimulationFilterData().word0;

	// SHAPE CREATION
	if (shape)
		shape->release();
	if (shape)
		owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->detachShape(*shape);

	owner->GetEngine()->GetPhysics()->DeleteActor(owner->GetComponent<ComponentRigidBody>()->GetRigidBody());

	ComponentTransform* currentTransform = owner->GetComponent<ComponentTransform>();
	float3 pos, scale;
	Quat quat;
	currentTransform->GetGlobalTransform().Decompose(pos, quat, scale);

	prevTransform = currentTransform;

	if (setFromAABB)
	{
		boxCollSize = owner->GetComponent<ComponentMesh>()->GetGlobalAABB().Size();
		setFromAABB = false;
	}
	
	physx::PxBoxGeometry boxGeometry(boxCollSize.x / 2, boxCollSize.y / 2, boxCollSize.z / 2);
	
	shape = owner->GetEngine()->GetPhysics()->GetPxPhysics()->createShape(boxGeometry, *owner->GetEngine()->GetPhysics()->GetPxMaterial());

	physx::PxTransform localPose;
	float3 center = owner->GetComponent<ComponentMesh>()->GetLocalAABB().CenterPoint();
	localPose.p = physx::PxVec3(offset.x, offset.y, offset.z - boxCollSize.z / 2);
	localPose.q = physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
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

		owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->attachShape(*shape);
	}


	owner->GetEngine()->GetPhysics()->AddActor(owner->GetComponent<ComponentRigidBody>()->GetRigidBody(), owner);
}

void ComponentCollider2::DrawCollider()
{
	if (drawCollider)
	{
		if (colliderShape == ColliderShape::BOX)
		{
			DrawBoxCollider();
		}
	}
}

void ComponentCollider2::DrawBoxCollider()
{
	float3 transformOffset = owner->GetComponent<ComponentTransform>()->GetPosition();
	physx::PxTransform localPose;
	physx::PxVec3 center;
	if (shape)
	{
		localPose = shape->getLocalPose();
		center = localPose.p;
	}

	float3 min = float3(center.x, center.y, center.z) - float3(boxCollSize.x / 2, boxCollSize.y / 2, boxCollSize.z / 2) + transformOffset;
	float3 max = float3(center.x, center.y, center.z) + float3(boxCollSize.x / 2, boxCollSize.y / 2, boxCollSize.z / 2) + transformOffset;

	glLineWidth(2.0f);
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

// Serialization
void ComponentCollider2::Save(Json &json) const
{
	json["type"] = "collider2";

	json["collider_type"] = (int)colliderShape;

	json["enabled"] = enabled;
	json["is_trigger"] = isTrigger;
	json["draw_collider"] = drawCollider;
	json["filter"] = filter;

	json["box_collider_size"] = {boxCollSize.x, boxCollSize.y, boxCollSize.z};
	json["offset"] = {offset.x, offset.y, offset.z};
}

void ComponentCollider2::Load(Json &json)
{
	colliderShape = (ColliderShape)json.at("collider_type");

	enabled = json.at("enabled");
	isTrigger = json.at("is_trigger");
	drawCollider = json.at("draw_collider");
	filter = json.at("filter");

	std::vector<float> values = json.at("box_collider_size").get<std::vector<float>>();
	boxCollSize = float3(values[0], values[1], values[2]);
	values.clear();
	/*values = json.at("offset").get<std::vector<float>>();
	offset = float3(values[0], values[1], values[2]);
	values.clear();*/

	setFromAABB = true;
	hasUpdated = true;
}

// On inspector draw
bool ComponentCollider2::InspectorDraw(PanelChooser* chooser)
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
const char* ComponentCollider2::ColliderShapeToString(const ColliderShape collShape)
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
const char *ComponentCollider2::CollisionLayerToString(const CollisionLayer collLayer)
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
