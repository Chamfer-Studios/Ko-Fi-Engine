#pragma once
#include "glmath.h"
#include "Color.h"
#include "glew.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Pyramid
};

class Primitive
{
public:
	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	Color color;
	mat4x4 transform;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class Cube : public Primitive
{
public :
	Cube();
	Cube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
	void DrawInterleavedMode();
public:
	vec3 size;
};

// ============================================
class Sphere : public Primitive
{
public:
	Sphere();
	Sphere(float radius);
	Sphere(float radius, unsigned int stacks, unsigned int sectors);
	void InnerRender() const;
public:
	float radius;
	unsigned int stacks;
	unsigned int sectors;
};

// ============================================
class Cylinder : public Primitive
{
public:
	Cylinder();
	Cylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class Line : public Primitive
{
public:
	Line();
	Line(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class Plane : public Primitive
{
public:
	Plane();
	Plane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};

// ============================================
class Pyramid : public Primitive
{
public:
	Pyramid();
	Pyramid(float baseX, float baseZ, float height);
	void InnerRender() const;
public:
	vec2 base;
	float height;
};