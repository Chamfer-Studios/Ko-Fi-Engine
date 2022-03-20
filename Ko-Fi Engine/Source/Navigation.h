#pragma once

#include "Module.h"
#include "Globals.h"

#include "glew.h"
#include "MathGeoLib/Math/float3.h"
#include <vector>

class GameObject;
class rcPolyMesh;
class rcPolyMeshDetail;
class dtNavMesh;
typedef unsigned int dtPolyRef;
class Mesh;

class Navigation : public Module
{
public:
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

	void FindPath(float3 origin, float3 destination, float3** path, int maxLength, int* actualLength);

	void OnGui() override;
private:
	rcPolyMeshDetail* ComputeNavmesh(Mesh* mesh);
	KoFiEngine* engine = nullptr;
	rcPolyMesh* navMesh = nullptr;
	rcPolyMeshDetail* navMeshDetail = nullptr;
	dtNavMesh* dtNavMesh = nullptr;
};