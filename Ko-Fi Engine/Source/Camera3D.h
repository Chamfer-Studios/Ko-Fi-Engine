#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class Input;

class Camera3D : public Module
{
public:
	Camera3D(Input* input);
	~Camera3D();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const vec3& Spot);
	void Move(const vec3& Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

public:

	vec3 X, Y, Z, Position, Reference;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
	Input* input = nullptr;
};