#pragma once
#include "Component.h"
#include "glmath.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* owner);
	~ComponentTransform();
	bool InspectorDraw(PanelChooser* chooser);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	vec3 GetPosition();
	vec3 GetRotation();
	vec3 GetScale();
	float* GetTransformMatrix();

private:
	vec3 position;
	vec3 rotation;
	vec3 scale;
	mat4x4 transform = IdentityMatrix;
};