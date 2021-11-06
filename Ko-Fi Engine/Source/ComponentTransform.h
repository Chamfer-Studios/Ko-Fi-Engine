#pragma once
#include "Component.h"
struct Vector {
	
	Vector(float x = 0.0f, float y = 0.0f,float z = 0.0f) :x(x), y(y), z(z) {}
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