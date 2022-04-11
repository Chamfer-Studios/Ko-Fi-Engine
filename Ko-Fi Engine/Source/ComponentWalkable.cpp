#include "ComponentWalkable.h"

ComponentWalkable::ComponentWalkable(GameObject* parent) : Component(parent)
{
	type = ComponentType::WALKABLE;
}

void ComponentWalkable::Save(Json& json) const
{
	json["type"] = "walkable";
}

void ComponentWalkable::Load(Json& json)
{
}

bool ComponentWalkable::Update(float dt)
{
	return true;
}

bool ComponentWalkable::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Walkable Surface", ImGuiTreeNodeFlags_AllowItemOverlap)) {
		DrawDeleteButton(owner, this);
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}
