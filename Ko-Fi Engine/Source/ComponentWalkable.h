#pragma once

#include "Component.h"

class KoFiEngine;

class ComponentWalkable : public Component
{
public:
	ComponentWalkable(GameObject* parent);

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;

	bool InspectorDraw(PanelChooser* chooser) override;
private:
};