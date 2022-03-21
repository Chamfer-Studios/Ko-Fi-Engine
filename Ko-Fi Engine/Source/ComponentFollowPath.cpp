#include "ComponentFollowPath.h"

#include "Engine.h"
#include "Navigation.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ComponentFollowPath::ComponentFollowPath(GameObject* parent) : Component(parent)
{
	type = ComponentType::FOLLOW_PATH;
	waypoints = (float3*)malloc(sizeof(float3) * nWaypoints);
	memset(waypoints, 0, sizeof(float3) * nWaypoints);
}

void ComponentFollowPath::Save(Json& json) const
{
	json["type"] = "followPath";

	std::vector<float> wpVector;

	for (int i = 0; i < nWaypoints; i++) {
		wpVector.push_back(waypoints[i].x);
		wpVector.push_back(waypoints[i].y);
		wpVector.push_back(waypoints[i].z);
	}

	json["waypoints"] = wpVector;
	json["pathfindLimit"] = pathfindLimit;
	json["speed"] = speed;
}

void ComponentFollowPath::Load(Json& json)
{
	std::vector<float> wpVector = json.at("waypoints").get<std::vector<float>>();

	nWaypoints = wpVector.size() / 3;
	free(waypoints);
	waypoints = (float3*)malloc(sizeof(float3) * nWaypoints);
	memset(waypoints, 0, sizeof(float3) * nWaypoints);

	for (int i = 0; i < wpVector.size(); i += 3) {
		waypoints[i / 3].x = wpVector[i + 0];
		waypoints[i / 3].y = wpVector[i + 1];
		waypoints[i / 3].z = wpVector[i + 2];
	}

	pathfindLimit = json.at("pathfindLimit").get<int>();
	speed = json.at("speed").get<float>();
}

bool ComponentFollowPath::Update(float dt)
{
	if (finalPath.size() == 0) return true;

	ComponentTransform* cTrans = owner->GetComponent<ComponentTransform>();
	float3 position = cTrans->GetPosition();
	float3 wp = finalPath[currentPathWaypoint];

	if (position.Distance(wp) < minWaypointTriggerDst) {
		currentPathWaypoint++;
		if (currentPathWaypoint >= finalPath.size()) currentPathWaypoint = 0;
	}

	float3 delta = finalPath[currentPathWaypoint] - cTrans->GetPosition();
	delta.Normalize();
	delta *= dt * speed;
	cTrans->SetPosition(cTrans->GetPosition() + delta);

	return true;
}

void ComponentFollowPath::CalculatePath()
{
	finalPath.clear();

	for (int i = 0; i < nWaypoints - 1; i++) {
		float3 current = waypoints[i];
		float3 next = waypoints[i + 1];
		float3* path = nullptr;
		int actualLength = 0;
		owner->GetEngine()->GetNavigation()->FindPath(current, next, &path, pathfindLimit, &actualLength);
		for (int j = 0; j < actualLength; j++) {
			finalPath.push_back(path[j]);
		}
	}

	appLog->AddLog("Found path with %d directions\n", finalPath.size());

	// Find closest waypoint to current position to resume patrol
	ComponentTransform* cTrans = owner->GetComponent<ComponentTransform>();
	int closestIndex = 0;
	float minDist = INT_MAX;
	for (int i = 0; i < finalPath.size(); i++) {
		float3 wp = finalPath[i];
		float dist = cTrans->GetPosition().Distance(wp);
		if (dist < minDist) {
			minDist = dist;
			closestIndex = i;
		}
	}
	currentPathWaypoint = closestIndex;
}

bool ComponentFollowPath::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Path Follower")) {
		ImGui::DragFloat("Speed", &speed, 0.5f);

		ImGui::DragInt("Pathfinder node limit", &pathfindLimit, 1.0f, 0);

		if (ImGui::Button("Generate path")) {
			CalculatePath();
		}

		if (ImGui::DragInt("Path length", &nWaypoints, 0.2f, 0)) {
			if (nWaypoints >= 0) {
				free(waypoints);
				waypoints = (float3*)malloc(sizeof(float3) * nWaypoints);
				memset(waypoints, 0, sizeof(float3) * nWaypoints);
			}
		}

		ImGui::Text("Waypoints: ");
		for (int i = 0; i < nWaypoints; i++) {
			std::string label = std::to_string(i);
			ImGui::DragFloat3(label.c_str(), &(waypoints[i][0]), 0.5f);
		}
	}

	return true;
}
