#include "ComponentWalkable.h"

ComponentWalkable::ComponentWalkable(GameObject* parent) : Component(parent)
{
	type = ComponentType::WALKABLE;
}

bool ComponentWalkable::Update(float dt)
{
	return true;
}

bool ComponentWalkable::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Walkable Surface")) {
		
	}

	return true;
}
