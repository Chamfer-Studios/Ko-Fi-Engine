#pragma once

#include "Module.h"
#include "Globals.h"

#include "glew.h"
#include <vector>

class GameObject;
class rcPolyMeshDetail;
class Mesh;

class Navigation : public Module
{
public:
	Navigation(KoFiEngine* engine);
	~Navigation();

	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	void OnNotify(const Event& event) override;

	void ComputeNavmesh();
	std::vector<GameObject*> CollectWalkableObjects();

	void OnGui() override;
private:
	rcPolyMeshDetail* ComputeNavmesh(Mesh* mesh);
	KoFiEngine* engine = nullptr;
	rcPolyMeshDetail* navMesh = nullptr;
};