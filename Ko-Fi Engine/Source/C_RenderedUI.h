#pragma once

#include "Component.h"

class C_RenderedUI : public Component {
public:
	C_RenderedUI(GameObject* parent) : Component(parent) {};
	virtual ~C_RenderedUI() {};

	virtual void Save(Json& json) const {};
	virtual void Load(Json& json) {};

	virtual bool Update(float dt) { return true; };
	virtual bool PostUpdate(float dt) { return true; };
	virtual bool InspectorDraw(PanelChooser* chooser) { return true; };

	virtual void Draw() = 0;
};