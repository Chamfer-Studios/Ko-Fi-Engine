#pragma once
#include "Component.h"
struct Vector {
	Vector() :x(0), y(0), z(0) {}
	float x, y, z;
};
class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();

private:
	Vector position;
	Vector rotation;
	Vector scale;
	
};