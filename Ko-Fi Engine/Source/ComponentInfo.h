#pragma once
#include "Component.h"
class ComponentInfo : public Component
{
public:
	ComponentInfo(GameObject* parent);
	~ComponentInfo();

	bool InspectorDraw(PanelChooser* chooser);

private:
};