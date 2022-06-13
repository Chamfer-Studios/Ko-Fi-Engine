#pragma once

#include "Component.h"

class KoFiEngine;

class C_Walkable : public Component
{
public:
	C_Walkable(GameObject* parent);
	virtual ~C_Walkable();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;

	bool InspectorDraw(PanelChooser* chooser) override;
private:
};