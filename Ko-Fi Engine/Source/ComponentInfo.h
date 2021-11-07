#pragma once
#include "Component.h"


class ComponentInfo : public Component
{
public:
	ComponentInfo(GameObject* owner);
	~ComponentInfo();

	bool InspectorDraw(PanelChooser* chooser);

private:
	const char* path = "";
};