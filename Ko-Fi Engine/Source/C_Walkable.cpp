#include "C_Walkable.h"

C_Walkable::C_Walkable(GameObject* parent) : Component(parent)
{
	type = ComponentType::WALKABLE;
}

void C_Walkable::Save(Json& json) const
{
	json["type"] = "walkable";
}

void C_Walkable::Load(Json& json)
{
}

bool C_Walkable::Update(float dt)
{
	return true;
}

bool C_Walkable::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Walkable Surface", ImGuiTreeNodeFlags_AllowItemOverlap)) {
		DrawDeleteButton(owner, this);
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}
