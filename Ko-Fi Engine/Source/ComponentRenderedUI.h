#pragma once

#include "Component.h"

class ComponentRenderedUI : public Component {
public:
	ComponentRenderedUI(GameObject* parent) : Component(parent) {};
	~ComponentRenderedUI() {};

	virtual void Save(Json& json) const {};
	virtual void Load(Json& json) {};

	virtual bool Update(float dt) { return true; };
	virtual bool PostUpdate(float dt) { return true; };
	virtual bool InspectorDraw(PanelChooser* chooser) { return true; };

	virtual void Draw() = 0;
};