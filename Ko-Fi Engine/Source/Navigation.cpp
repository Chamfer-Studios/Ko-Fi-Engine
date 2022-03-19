#include "Navigation.h"

#include "Recast.h"

#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"

#include "ComponentWalkable.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Mesh.h"

Navigation::Navigation(KoFiEngine* engine) : Module()
{
	name = "Navigation";
	this->engine = engine;
}

Navigation::~Navigation()
{
}

bool Navigation::Start()
{
	return true;
}

bool Navigation::PreUpdate(float dt)
{
	return true;
}

bool Navigation::Update(float dt)
{
	return true;
}

bool Navigation::PostUpdate(float dt)
{
	// http://www.stevefsp.org/projects/rcndoc/prod/structrcPolyMeshDetail.html

	if (navMesh == nullptr) return true;

	glBegin(GL_LINES);
	glLineWidth(2.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glDisable(GL_LIGHTING);

	for (int i = 0; i < navMesh->nmeshes; ++i)
	{
		const unsigned int* meshDef = &navMesh->meshes[i * 4];
		const unsigned int baseVerts = meshDef[0];
		const unsigned int baseTri = meshDef[2];
		const int ntris = (int)meshDef[3];

		const float* verts = &navMesh->verts[baseVerts * 3];
		const unsigned char* tris = &navMesh->tris[baseTri * 4];
		// Iterate the sub-mesh's triangles.
		for (int j = 0; j < ntris; ++j)
		{
			const float x1 = verts[tris[j * 4 + 0] * 3 + 0];
			const float y1 = verts[tris[j * 4 + 0] * 3 + 1];
			const float z1 = verts[tris[j * 4 + 0] * 3 + 2];
			const float x2 = verts[tris[j * 4 + 1] * 3 + 0];
			const float y2 = verts[tris[j * 4 + 1] * 3 + 1];
			const float z2 = verts[tris[j * 4 + 1] * 3 + 2];
			const float x3 = verts[tris[j * 4 + 2] * 3 + 0];
			const float y3 = verts[tris[j * 4 + 2] * 3 + 1];
			const float z3 = verts[tris[j * 4 + 2] * 3 + 2];

			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			glVertex3f(x2, y2, z2);
			glVertex3f(x3, y3, z3);
			glVertex3f(x3, y3, z3);
			glVertex3f(x1, y1, z1);
		}
	}

	glEnable(GL_LIGHTING);
	glEnd();

	return true;
}

bool Navigation::CleanUp()
{
	return true;
}

void Navigation::OnNotify(const Event& event)
{

}

void Navigation::ComputeNavmesh()
{
	std::vector<GameObject*> walkableObjects = CollectWalkableObjects();
	std::vector<Mesh*> meshes;
	std::vector<float4x4> transforms;

	for (auto go : walkableObjects) {
		ComponentMesh* cMesh = go->GetComponent<ComponentMesh>();
		ComponentTransform* cTransform = go->GetComponent<ComponentTransform>();
		if (cMesh != nullptr && cTransform != nullptr) {
			meshes.push_back(cMesh->GetMesh());
			transforms.push_back(cTransform->GetGlobalTransform());
		}
	}

	Mesh* combined = Mesh::MeshUnion(meshes, transforms);

	navMesh = ComputeNavmesh(combined);
}

rcPolyMeshDetail* Navigation::ComputeNavmesh(Mesh* mesh)
{
	// https://wiki.jmonkeyengine.org/docs/3.4/contributions/ai/recast.html
	// https://github.com/recastnavigation/recastnavigation/blob/c5cbd53024c8a9d8d097a4371215e3342d2fdc87/RecastDemo/Source/Sample_SoloMesh.cpp

	rcConfig* config = new rcConfig();

	float* vertices = mesh->vertices;
	int nv = mesh->verticesSizeBytes / (sizeof(float) * 3);
	int* tris = (int*)mesh->indices;
	int nt = mesh->indicesSizeBytes / sizeof(uint) / 3;

	float bMin[3] = {0, 0, 0};
	float bMax[3] = {0, 0, 0};
	rcCalcBounds(vertices, mesh->verticesSizeBytes / (sizeof(float) * 3), bMin, bMax);

	config->bmin[0] = bMin[0];
	config->bmin[1] = bMin[1];
	config->bmin[2] = bMin[2];
	config->bmax[0] = bMax[0];
	config->bmax[1] = bMax[1];
	config->bmax[2] = bMax[2];

	config->cs = 10.f;
	config->ch = 3.f;
	config->walkableSlopeAngle = 45;
	config->walkableClimb = 1;
	config->walkableHeight = 2;
	config->walkableRadius = 2;
	config->minRegionArea = 2.f;
	config->mergeRegionArea = 2.f;
	config->borderSize = 1.5f;
	config->maxEdgeLen = 30.f;
	config->maxVertsPerPoly = 12;
	config->detailSampleMaxError = 1.0f;
	config->detailSampleDist = 1.0f;

	int w, h;
	rcCalcGridSize(bMin, bMax, config->cs, &w, &h);

	rcContext* context = new rcContext();

	rcHeightfield* heightfield = rcAllocHeightfield();
	if (!rcCreateHeightfield(context, *heightfield, w, h, bMin, bMax, config->cs, config->ch)) {
		appLog->AddLog("Could not create heightfield!\n");
		return nullptr;
	}

	unsigned char* areas = new unsigned char[nt];
	memset(areas, 0, nt * sizeof(unsigned char));
	rcMarkWalkableTriangles(context, config->walkableSlopeAngle, vertices, nv, tris, nt, areas);
	if (!rcRasterizeTriangles(context, vertices, nv, tris, areas, nt, *heightfield, config->walkableClimb)) {
		appLog->AddLog("Could not rasterize triangles!");
	}
	rcFilterLowHangingWalkableObstacles(context, config->walkableClimb, *heightfield);
	rcFilterLedgeSpans(context, config->walkableHeight, config->walkableClimb, *heightfield);
	rcFilterWalkableLowHeightSpans(context, config->walkableHeight, *heightfield);

	rcCompactHeightfield* compactHeightfield = rcAllocCompactHeightfield();
	if (!rcBuildCompactHeightfield(context, config->walkableHeight, config->walkableClimb, *heightfield, *compactHeightfield)) {
		appLog->AddLog("Could not build compact data!\n");
		return nullptr;
	}

	if (!rcErodeWalkableArea(context, config->walkableRadius, *compactHeightfield)) {
		appLog->AddLog("Could not erode!\n");
		return nullptr;
	}

	if (!rcBuildDistanceField(context, *compactHeightfield)) {
		appLog->AddLog("Could not build distance field!\n");
		return nullptr;
	}

	if (!rcBuildRegions(context, *compactHeightfield, config->borderSize, config->minRegionArea, config->mergeRegionArea)) {
		appLog->AddLog("Could not build watershed regions!\n");
		return nullptr;
	}

	rcContourSet* contourSet = rcAllocContourSet();
	if (!rcBuildContours(context, *compactHeightfield, 2.0f, config->maxEdgeLen, *contourSet)) {
		appLog->AddLog("Could not create contours!\n");
		return nullptr;
	}

	rcPolyMesh* polyMesh = rcAllocPolyMesh();
	if (!rcBuildPolyMesh(context, *contourSet, config->maxVertsPerPoly, *polyMesh)) {
		appLog->AddLog("Could not triangulate contours!\n");
		return nullptr;
	}

	rcPolyMeshDetail* polyMeshDetail = rcAllocPolyMeshDetail();
	if (!rcBuildPolyMeshDetail(context, *polyMesh, *compactHeightfield, config->detailSampleDist, config->detailSampleMaxError, *polyMeshDetail)) {
		appLog->AddLog("Could not build detail mesh!\n");
		return nullptr;
	}

	return polyMeshDetail;
}

std::vector<GameObject*> Navigation::CollectWalkableObjects()
{
	std::vector<GameObject*> list = engine->GetSceneManager()->GetCurrentScene()->gameObjectList;
	std::vector<GameObject*> res;

	for (auto o : list) {
		if (o->GetComponent<ComponentTransform>() != nullptr) {
			res.push_back(o);
		}
	}

	return res;
}

void Navigation::OnGui()
{
}
