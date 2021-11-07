#pragma once
#include "Component.h"
#include "glmath.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();
	bool InspectorDraw(PanelChooser* chooser);

	vec3 GetPosition();
	vec3 GetRotation();
	vec3 GetScale();

private:
	vec3 position;
	vec3 rotation;
	vec3 scale;
};