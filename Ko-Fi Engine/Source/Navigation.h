#pragma once

#include "Module.h"
#include "Globals.h"

#include "glew.h"
#include "MathGeoLib/Math/float3.h"
#include <vector>
#include <tuple>
#include "json.hpp"

class GameObject;
class rcPolyMesh;
class rcPolyMeshDetail;
class dtNavMesh;
typedef unsigned int dtPolyRef;
class Mesh;

class Navigation : public Module
{
public:
	struct NavMeshConfig {
		float cs = .3f;
		float ch = .2f;
		float walkableSlopeAngle = 45;
		float walkableClimb = 1.0f;
		int walkableHeight = 2;
		float walkableRadius = 2.f;
		float minRegionArea = 2.f;
		float mergeRegionArea = 2.f;
		float borderSize = 0.5f;
		float maxEdgeLen = 30.f;
		int maxVertsPerPoly = 6;
		float detailSampleMaxError = 1.0f;
		float detailSampleDist = 1.0f;
	};

	enum PolyAreas
	{
		SAMPLE_POLYAREA_GROUND,
	};

	enum PolyFlags
	{
		SAMPLE_POLYFLAGS_WALK = 0x01,
	};

	Navigation(KoFiEngine* engine);
	~Navigation();

	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	void OnNotify(const Event& event) override;

	void ComputeNavmesh();
	void PrepareDetour();
	std::vector<GameObject*> CollectWalkableObjects();

	std::tuple<std::vector<float3>> FindPath(float3 origin, float3 destination, int maxPolyLength, int maxVectorLength);

	void Save(Json& json) const;
	void Load(Json& json);

	void OnGui() override;
private:
	rcPolyMeshDetail* ComputeNavmesh(Mesh* mesh);
	KoFiEngine* engine = nullptr;
	rcPolyMesh* navMesh = nullptr;
	rcPolyMeshDetail* navMeshDetail = nullptr;
	dtNavMesh* dtNavMesh = nullptr;
	NavMeshConfig navMeshConfig;
};