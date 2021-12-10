#pragma once
#include "Component.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent);
	~ComponentCamera();

	bool InspectorDraw(PanelChooser* chooser);

private:
};