#pragma once
#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Component.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "MathGeoLib/Geometry/Frustum.h"

class GameObject;

enum class FixedFOV
{
	HORIZONTAL,
	VERTICAL
};

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent);
	~ComponentCamera();

	bool Update() override;
	void DrawFrustum() const;
	bool InspectorDraw(PanelChooser* chooser);

private:
	AABB bbox;
	AABB drawingBbox;
	Frustum frustum;
	FixedFOV fixedFOV;
	bool mainCamera;
	bool drawFrustum, drawBBox;
	bool culling;
	float horizontalFOV = 60.0f;
	float aspectRatio = 1.777f; //16/9
};

#endif // __COMPONENT_CAMERA_H__