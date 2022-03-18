#include "Navigation.h"

#include "Recast.h"

#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"

#include "ComponentWalkable.h"
#include "ComponentMesh.h"
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

	return true;

	if (navMesh == nullptr) return true;

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);

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
			const float x = verts[tris[j * 4 + 0] * 3];
			const float y = verts[tris[j * 4 + 1] * 3];
			const float z = verts[tris[j * 4 + 2] * 3];
			glVertex3f(x, y, z);
		}
	}

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
	// https://wiki.jmonkeyengine.org/docs/3.4/contributions/ai/recast.html
	// https://github.com/recastnavigation/recastnavigation/blob/c5cbd53024c8a9d8d097a4371215e3342d2fdc87/RecastDemo/Source/Sample_SoloMesh.cpp

	rcConfig* config = new rcConfig();

	std::vector<GameObject*> walkableMeshes = CollectWalkableMeshes();
	if (walkableMeshes.size() == 0) return;

	GameObject* walkable = walkableMeshes[0];
	ComponentMesh* cMesh = walkable->GetComponent<ComponentMesh>();
	float* vertices = cMesh->GetMesh()->vertices;
	int nv = cMesh->GetMesh()->verticesSizeBytes / (sizeof(float) * 3);
	int* tris = (int*)cMesh->GetMesh()->indices;
	int nt = cMesh->GetMesh()->indicesSizeBytes / sizeof(uint) / 3;

	float bMin[3] = {0, 0, 0};
	float bMax[3] = {0, 0, 0};
	rcCalcBounds(cMesh->GetMesh()->vertices, cMesh->GetMesh()->verticesSizeBytes / (sizeof(float) * 3), bMin, bMax);
	if (bMin != nullptr && bMax != nullptr) {
		appLog->AddLog("%f, %f, %f ... %f, %f, %f\n", bMin[0], bMin[1], bMin[2], bMax[0], bMax[1], bMax[2]);
	}

	config->bmin[0] = bMin[0];
	config->bmin[1] = bMin[1];
	config->bmin[2] = bMin[2];
	config->bmax[0] = bMax[0];
	config->bmax[1] = bMax[1];
	config->bmax[2] = bMax[2];

	config->cs = 0.3f;
	config->ch = 0.2f;
	config->walkableSlopeAngle = 45;
	config->walkableClimb = 1;
	config->walkableHeight = 2;
	config->walkableRadius = 2;
	config->minRegionArea = 0.5f;
	config->mergeRegionArea = 1.5f;
	config->borderSize = 1.5f;
	config->maxEdgeLen = 1.2f;
	config->maxVertsPerPoly = 6;
	config->detailSampleMaxError = 1.0f;
	config->detailSampleDist = 1.0f;

	int w, h;
	rcCalcGridSize(bMin, bMax, config->cs, &w, &h);

	rcContext* context = new rcContext();

	rcHeightfield* heightfield = rcAllocHeightfield();
	if (!rcCreateHeightfield(context, *heightfield, w, h, bMin, bMax, config->cs, config->ch)) {
		appLog->AddLog("Could not create heightfield!\n");
		return;
	}

	unsigned char* areas = new unsigned char[nt];
	memset(areas, 0, nt * sizeof(unsigned char));
	rcMarkWalkableTriangles(context, config->walkableSlopeAngle, vertices, nv, tris, nt, areas);
	if (!rcRasterizeTriangles(context, vertices, areas, nt, *heightfield)) {
		appLog->AddLog("Could not rasterize triangles!");
	}
	rcFilterLowHangingWalkableObstacles(context, config->walkableClimb, *heightfield);
	rcFilterLedgeSpans(context, config->walkableHeight, config->walkableClimb, *heightfield);
	rcFilterWalkableLowHeightSpans(context, config->walkableHeight, *heightfield);

	rcCompactHeightfield* compactHeightfield = rcAllocCompactHeightfield();
	if (!rcBuildCompactHeightfield(context, config->walkableHeight, config->walkableClimb, *heightfield, *compactHeightfield)) {
		appLog->AddLog("Could not build compact data!\n");
		return;
	}

	if (!rcErodeWalkableArea(context, config->walkableRadius, *compactHeightfield)) {
		appLog->AddLog("Could not erode!\n");
		return;
	}

	if (!rcBuildDistanceField(context, *compactHeightfield)) {
		appLog->AddLog("Could not build distance field!\n");
		return;
	}

	if (!rcBuildRegions(context, *compactHeightfield, config->borderSize, config->minRegionArea, config->mergeRegionArea)) {
		appLog->AddLog("Could not build watershed regions!\n");
		return;
	}

	rcContourSet* contourSet = rcAllocContourSet();
	if (!rcBuildContours(context, *compactHeightfield, 2.0f, config->maxEdgeLen, *contourSet)) {
		appLog->AddLog("Could not create contours!\n");
		return;
	}

	rcPolyMesh* polyMesh = rcAllocPolyMesh();
	if (!rcBuildPolyMesh(context, *contourSet, config->maxVertsPerPoly, *polyMesh)) {
		appLog->AddLog("Could not triangulate contours!\n");
		return;
	}

	rcPolyMeshDetail* polyMeshDetail = rcAllocPolyMeshDetail();
	if (!rcBuildPolyMeshDetail(context, *polyMesh, *compactHeightfield, config->detailSampleDist, config->detailSampleMaxError, *polyMeshDetail)) {
		appLog->AddLog("Could not build detail mesh!\n");
		return;
	}

	navMesh = polyMeshDetail;
}

std::vector<GameObject*> Navigation::CollectWalkableMeshes()
{
	std::vector<GameObject*> list = engine->GetSceneManager()->GetCurrentScene()->gameObjectList;
	std::vector<GameObject*> res;

	for (auto o : list) {
		if (o->GetComponent<ComponentWalkable>() != nullptr) {
			res.push_back(o);
		}
	}

	return res;
}

void Navigation::GameObjectMeshUnion(std::vector<GameObject*> objects, float** vertices, int* nv)
{
	*nv = 0;
	for (auto o : objects) {
		ComponentMesh* cMesh = o->GetComponent<ComponentMesh>();
		if (cMesh == nullptr) continue;
		*nv += cMesh->GetMesh()->verticesSizeBytes / (sizeof(float) * 3);
	}

	*vertices = (float*)malloc(*nv * sizeof(float) * 3);

	unsigned int index = 0;
	for (auto o : objects) {
		ComponentMesh* cMesh = o->GetComponent<ComponentMesh>();
		if (cMesh == nullptr) continue;
		memcpy(*vertices + index, cMesh->GetMesh()->vertices, cMesh->GetMesh()->verticesSizeBytes);
		index += cMesh->GetMesh()->verticesSizeBytes;
	}
}

void Navigation::OnGui()
{
}
