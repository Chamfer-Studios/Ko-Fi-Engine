#pragma once
#include "Component.h"
struct Vector {
	
	float x, y, z;
};
class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();
	bool InspectorDraw();

private:
	Vector position;
	Vector rotation;
	Vector scale;
	
};