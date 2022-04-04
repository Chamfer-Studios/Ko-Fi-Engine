#pragma once

#include "Component.h"

#include "MathGeoLib/Math/float3.h"

class KoFiEngine;

class ComponentFollowPath : public Component
{
public:
	ComponentFollowPath(GameObject* parent);

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;

	void CalculatePath();

	bool InspectorDraw(PanelChooser* chooser) override;
private:
	int nWaypoints = 0;
	float3* waypoints = nullptr;

	int pathfindLimit = 1000;

	std::vector<float3> finalPath;
	int currentPathWaypoint = 0;
	float minWaypointTriggerDst = 2;
	float speed = 1.0f;
};