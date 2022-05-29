#include "M_Navigation.h"

#include "Recast.h"

// Modules
#include "Engine.h"
#include "M_SceneManager.h"
#include "Scene.h"

// GameObject
#include "GameObject.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Walkable.h"

#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourNavMesh.h"
#include "R_Mesh.h"
#include "JsonHandler.h"

#include <lua.hpp>
#include <sol.hpp>
#include <vector>
#include <queue>
#include <tuple>

#include "optick.h"

M_Navigation::M_Navigation(KoFiEngine* engine) : Module()
{
	name = "Navigation";
	this->engine = engine;
}

M_Navigation::~M_Navigation()
{
}

bool M_Navigation::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool M_Navigation::Start()
{
	return true;
}

bool M_Navigation::PreUpdate(float dt)
{
	return true;
}

bool M_Navigation::Update(float dt)
{
	return true;
}

bool M_Navigation::PostUpdate(float dt)
{
	return true;
}

bool M_Navigation::CleanUp()
{
	return true;
}

void M_Navigation::OnNotify(const Event& event)
{

}

void M_Navigation::ComputeNavmesh()
{
	std::vector<GameObject*> walkableObjects;
	CollectWalkableObjects(engine->GetSceneManager()->GetCurrentScene()->rootGo, walkableObjects, false);
	std::vector<R_Mesh*> meshes;
	std::vector<float4x4> transforms;

	for (auto go : walkableObjects) {
		C_Mesh* cMesh = go->GetComponent<C_Mesh>();
		C_Transform* cTransform = go->GetComponent<C_Transform>();
		if (cMesh != nullptr && cTransform != nullptr) {
			meshes.push_back(cMesh->GetMesh());
			transforms.push_back(cTransform->GetGlobalTransform());
		}
	}

	R_Mesh* combined = R_Mesh::MeshUnion(meshes, transforms);

	navMeshDetail = ComputeNavmesh(combined);
}

void M_Navigation::PrepareDetour()
{
	if (navMeshDetail == nullptr) return;

	if (dtNavMesh != nullptr) dtFreeNavMesh(dtNavMesh);

	for (int i = 0; i < navMesh->npolys; ++i)
	{
		if (navMesh->areas[i] == RC_WALKABLE_AREA)
			navMesh->areas[i] = SAMPLE_POLYAREA_GROUND;

		if (navMesh->areas[i] == SAMPLE_POLYAREA_GROUND)
		{
			navMesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
		}
	}

	dtNavMeshCreateParams dtNavMeshCreateParams;
	memset(&dtNavMeshCreateParams, 0, sizeof(dtNavMeshCreateParams));
	dtNavMeshCreateParams.verts = navMesh->verts;
	dtNavMeshCreateParams.vertCount = navMesh->nverts;
	dtNavMeshCreateParams.polys = navMesh->polys;
	dtNavMeshCreateParams.polyAreas = navMesh->areas;
	dtNavMeshCreateParams.polyFlags = navMesh->flags;
	dtNavMeshCreateParams.polyCount = navMesh->npolys;
	dtNavMeshCreateParams.nvp = navMesh->nvp;

	dtNavMeshCreateParams.detailMeshes = navMeshDetail->meshes;
	dtNavMeshCreateParams.detailVerts = navMeshDetail->verts;
	dtNavMeshCreateParams.detailVertsCount = navMeshDetail->nverts;
	dtNavMeshCreateParams.detailTris = navMeshDetail->tris;
	dtNavMeshCreateParams.detailTriCount = navMeshDetail->ntris;

	dtNavMeshCreateParams.offMeshConCount = 0;

	dtNavMeshCreateParams.walkableHeight = navMeshConfig.walkableHeight;
	dtNavMeshCreateParams.walkableClimb = navMeshConfig.walkableClimb;
	dtNavMeshCreateParams.walkableRadius = navMeshConfig.walkableRadius;

	rcVcopy(dtNavMeshCreateParams.bmin, navMesh->bmin);
	rcVcopy(dtNavMeshCreateParams.bmax, navMesh->bmax);
	dtNavMeshCreateParams.cs = navMeshConfig.cs;
	dtNavMeshCreateParams.ch = navMeshConfig.ch;
	dtNavMeshCreateParams.buildBvTree = true;

	unsigned char* data = nullptr;
	int dataSize = 0;
	dtCreateNavMeshData(&dtNavMeshCreateParams, &data, &dataSize);

	if (dataSize == 0) return;

	dtNavMesh = dtAllocNavMesh();
	if (!dtStatusSucceed(dtNavMesh->init(data, dataSize, DT_TILE_FREE_DATA))) {
		appLog->AddLog("Could not create DT navmesh!");
		return;
	}

	appLog->AddLog("Sucessfully prepared pathfinding.\n");
}

rcPolyMeshDetail* M_Navigation::ComputeNavmesh(R_Mesh* mesh)
{
	// https://wiki.jmonkeyengine.org/docs/3.4/contributions/ai/recast.html
	// https://github.com/recastnavigation/recastnavigation/blob/c5cbd53024c8a9d8d097a4371215e3342d2fdc87/RecastDemo/Source/Sample_SoloMesh.cpp

	if (navMesh != nullptr) rcFreePolyMesh(navMesh);
	if (navMeshDetail != nullptr) rcFreePolyMeshDetail(navMeshDetail);

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

	config->cs = navMeshConfig.cs;
	config->ch = navMeshConfig.ch;
	config->walkableSlopeAngle = navMeshConfig.walkableSlopeAngle;
	config->walkableClimb = navMeshConfig.walkableClimb;
	config->walkableHeight = navMeshConfig.walkableHeight;
	config->walkableRadius = navMeshConfig.walkableRadius;
	config->minRegionArea = navMeshConfig.minRegionArea;
	config->mergeRegionArea = navMeshConfig.mergeRegionArea;
	config->borderSize = navMeshConfig.borderSize;
	config->maxEdgeLen = navMeshConfig.maxEdgeLen;
	config->maxVertsPerPoly = navMeshConfig.maxVertsPerPoly;
	config->detailSampleMaxError = navMeshConfig.detailSampleMaxError;
	config->detailSampleDist = navMeshConfig.detailSampleDist;

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
		return nullptr;
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

	navMesh = rcAllocPolyMesh();
	if (!rcBuildPolyMesh(context, *contourSet, config->maxVertsPerPoly, *navMesh)) {
		appLog->AddLog("Could not triangulate contours!\n");
		return nullptr;
	}

	rcPolyMeshDetail* polyMeshDetail = rcAllocPolyMeshDetail();
	if (!rcBuildPolyMeshDetail(context, *navMesh, *compactHeightfield, config->detailSampleDist, config->detailSampleMaxError, *polyMeshDetail)) {
		appLog->AddLog("Could not build detail mesh!\n");
		return nullptr;
	}

	delete config;
	delete context;

	rcFreeHeightField(heightfield);
	delete[] areas;
	rcFreeCompactHeightfield(compactHeightfield);
	rcFreeContourSet(contourSet);
	delete mesh;

	appLog->AddLog("Successfully created navmesh.\n");

	return polyMeshDetail;
}

void M_Navigation::CollectWalkableObjects(GameObject* go, std::vector<GameObject*>& res, bool force)
{
	if (force || go->GetComponent<C_Walkable>() != nullptr) {
		res.push_back(go);
		for (auto c : go->children) {
			CollectWalkableObjects(c, res, true);
		}
	}
	else {
		for (auto c : go->children) {
			CollectWalkableObjects(c, res, false);
		}
	}
}

bool M_Navigation::DrawNavmesh()
{
	if (!drawNavmesh) return true;

	OPTICK_EVENT();

	// http://www.stevefsp.org/projects/rcndoc/prod/structrcPolyMeshDetail.html

	if (navMeshDetail == nullptr) return true;

	glBegin(GL_LINES);
	glLineWidth(2.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glDisable(GL_LIGHTING);

	for (int i = 0; i < navMeshDetail->nmeshes; ++i)
	{
		const unsigned int* meshDef = &navMeshDetail->meshes[i * 4];
		const unsigned int baseVerts = meshDef[0];
		const unsigned int baseTri = meshDef[2];
		const int ntris = (int)meshDef[3];

		const float* verts = &navMeshDetail->verts[baseVerts * 3];
		const unsigned char* tris = &navMeshDetail->tris[baseTri * 4];
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

std::tuple<std::vector<float3>> M_Navigation::FindPath(float3 origin, float3 destination, int maxPolyLength, int maxVectorLength)
{
	std::vector<float3> path;
	if (dtNavMesh == nullptr) return std::make_tuple(path);
	int actualPolyLength = 0;
	int actualVectorLength = 0;

	dtNavMeshQuery* query = dtAllocNavMeshQuery();

	query->init(dtNavMesh, maxPolyLength);

	dtQueryFilter* filter = new dtQueryFilter();

	dtPolyRef originPoly;
	float originArray[3] = { origin.x, origin.y, origin.z };
	float originPolyPos[3] = { 0, 0, 0 };
	query->findNearestPoly(originArray, navMeshConfig.extents, filter, &originPoly, originPolyPos);

	dtPolyRef destinationPoly;
	float destinationArray[3] = { destination.x, destination.y, destination.z };
	float destinationPolyPos[3] = { 0, 0, 0 };
	query->findNearestPoly(destinationArray, navMeshConfig.extents, filter, &destinationPoly, destinationPolyPos);

	dtPolyRef* polyPath = (dtPolyRef*)malloc(sizeof(dtPolyRef*) * maxPolyLength);
	memset(polyPath, 0, sizeof(dtPolyRef) * maxPolyLength);

	query->findPath(originPoly, destinationPoly, originPolyPos, destinationPolyPos, filter, polyPath, &actualPolyLength, maxPolyLength);

	float* vectorPath = (float*)malloc(sizeof(float*) * maxVectorLength);
	memset(vectorPath, 0, sizeof(float*) * maxVectorLength);
	query->findStraightPath(origin.ptr(), destination.ptr(), polyPath, actualPolyLength, vectorPath, nullptr, nullptr, &actualVectorLength, maxVectorLength);

	for (int i = 0; i < actualVectorLength; i++) {
		path.push_back(float3(vectorPath[i * 3], vectorPath[i * 3 + 1], vectorPath[i * 3 + 2]));
	}

	dtFreeNavMeshQuery(query);
	delete filter;
	free(polyPath);
	free(vectorPath);

	return std::make_tuple(path);
}

void M_Navigation::Save(Json& json) const
{
	if (navMesh == nullptr)
		return;

	Json navmeshJson = Json::object();

	navmeshJson.emplace("verts", Json::array());
	std::vector<unsigned short> verts;
	for (int i = 0; i < navMesh->nverts * 3; i++) {
		verts.push_back(navMesh->verts[i]);
	}
	navmeshJson["verts"] = verts;

	navmeshJson.emplace("polys", Json::array());
	std::vector<unsigned short> polys;
	for (int i = 0; i < navMesh->maxpolys * 2 * navMesh->nvp; i++) {
		polys.push_back(navMesh->polys[i]);
	}
	navmeshJson["polys"] = polys;

	navmeshJson.emplace("regs", Json::array());
	std::vector<unsigned short> regs;
	for (int i = 0; i < navMesh->maxpolys; i++) {
		regs.push_back(navMesh->regs[i]);
	}
	navmeshJson["regs"] = regs;

	navmeshJson.emplace("flags", Json::array());
	std::vector<unsigned short> flags;
	for (int i = 0; i < navMesh->maxpolys; i++) {
		flags.push_back(navMesh->flags[i]);
	}
	navmeshJson["flags"] = flags;

	navmeshJson.emplace("areas", Json::array());
	std::vector<unsigned char> areas;
	for (int i = 0; i < navMesh->maxpolys; i++) {
		areas.push_back(navMesh->areas[i]);
	}
	navmeshJson["areas"] = areas;

	navmeshJson.emplace("nverts", navMesh->nverts);
	navmeshJson.emplace("npolys", navMesh->npolys);
	navmeshJson.emplace("maxpolys", navMesh->maxpolys);
	navmeshJson.emplace("nvp", navMesh->nvp);

	navmeshJson.emplace("bmin", Json::array());
	navmeshJson["bmin"] = {
		navMesh->bmin[0],
		navMesh->bmin[1],
		navMesh->bmin[2],
	};

	navmeshJson.emplace("bmax", Json::array());
	navmeshJson["bmax"] = {
		navMesh->bmax[0],
		navMesh->bmax[1],
		navMesh->bmax[2],
	};

	navmeshJson.emplace("cs", navMesh->cs);
	navmeshJson.emplace("ch", navMesh->ch);
	navmeshJson.emplace("borderSize", navMesh->borderSize);
	navmeshJson.emplace("maxEdgeError", navMesh->maxEdgeError);

	Json detailNavmeshJson = Json::object();

	detailNavmeshJson.emplace("meshes", Json::array());
	std::vector<unsigned int> meshes;
	for (int i = 0; i < navMeshDetail->nmeshes * 4; i++) {
		meshes.push_back(navMeshDetail->meshes[i]);
	}
	detailNavmeshJson["meshes"] = meshes;

	detailNavmeshJson.emplace("verts", Json::array());
	std::vector<float> dverts;
	for (int i = 0; i < navMeshDetail->nverts * 3; i++) {
		dverts.push_back(navMeshDetail->verts[i]);
	}
	detailNavmeshJson["verts"] = dverts;

	detailNavmeshJson.emplace("tris", Json::array());
	std::vector<unsigned char> tris;
	for (int i = 0; i < navMeshDetail->ntris * 4; i++) {
		tris.push_back(navMeshDetail->tris[i]);
	}
	detailNavmeshJson["tris"] = tris;

	detailNavmeshJson.emplace("nmeshes", navMeshDetail->nmeshes);
	detailNavmeshJson.emplace("nverts", navMeshDetail->nverts);
	detailNavmeshJson.emplace("ntris", navMeshDetail->ntris);

	json.emplace("basic", navmeshJson);
	json.emplace("detail", detailNavmeshJson);
}

void M_Navigation::Load(Json& json)
{
	if (json.find("basic") == json.end() || json.find("detail") == json.end()) return;

	navMesh = rcAllocPolyMesh();
	navMeshDetail = rcAllocPolyMeshDetail();

	Json navmeshJson = json.at("basic");
	Json detailNavmeshJson = json.at("detail");

	std::vector<unsigned short> verts = navmeshJson.at("verts").get<std::vector<unsigned short>>();
	navMesh->verts = (unsigned short*)malloc(sizeof(unsigned short) * verts.size());
	for (int i = 0; i < verts.size(); i++) {
		navMesh->verts[i] = verts[i];
	}

	std::vector<unsigned short> polys = navmeshJson.at("polys").get<std::vector<unsigned short>>();
	navMesh->polys = (unsigned short*)malloc(sizeof(unsigned short) * polys.size());
	for (int i = 0; i < polys.size(); i++) {
		navMesh->polys[i] = polys[i];
	}

	std::vector<unsigned short> regs = navmeshJson.at("regs").get<std::vector<unsigned short>>();
	navMesh->regs = (unsigned short*)malloc(sizeof(unsigned short) * regs.size());
	for (int i = 0; i < regs.size(); i++) {
		navMesh->regs[i] = regs[i];
	}

	std::vector<unsigned short> flags = navmeshJson.at("flags").get<std::vector<unsigned short>>();
	navMesh->flags = (unsigned short*)malloc(sizeof(unsigned short) * flags.size());
	for (int i = 0; i < flags.size(); i++) {
		navMesh->flags[i] = flags[i];
	}

	std::vector<unsigned char> areas = navmeshJson.at("areas").get<std::vector<unsigned char>>();
	navMesh->areas = (unsigned char*)malloc(sizeof(unsigned char) * areas.size());
	for (int i = 0; i < areas.size(); i++) {
		navMesh->areas[i] = areas[i];
	}

	navMesh->nverts = navmeshJson.at("nverts").get<int>();
	navMesh->npolys = navmeshJson.at("npolys").get<int>();
	navMesh->maxpolys = navmeshJson.at("maxpolys").get<int>();
	navMesh->nvp = navmeshJson.at("nvp").get<int>();

	std::vector<float> bmin = navmeshJson.at("bmin").get<std::vector<float>>();
	navMesh->bmin[0] = bmin[0];
	navMesh->bmin[1] = bmin[1];
	navMesh->bmin[2] = bmin[2];

	std::vector<float> bmax = navmeshJson.at("bmax").get<std::vector<float>>();
	navMesh->bmax[0] = bmax[0];
	navMesh->bmax[1] = bmax[1];
	navMesh->bmax[2] = bmax[2];

	navMesh->cs = navmeshJson.at("cs").get<float>();
	navMesh->ch = navmeshJson.at("ch").get<float>();
	navMesh->borderSize = navmeshJson.at("borderSize").get<int>();
	navMesh->maxEdgeError = navmeshJson.at("maxEdgeError").get<float>();

	std::vector<unsigned int> meshes = detailNavmeshJson.at("meshes").get<std::vector<unsigned int>>();
	navMeshDetail->meshes = (unsigned int*)malloc(sizeof(unsigned int) * meshes.size());
	for (int i = 0; i < meshes.size(); i++) {
		navMeshDetail->meshes[i] = meshes[i];
	}

	std::vector<float> dverts = detailNavmeshJson.at("verts").get<std::vector<float>>();
	navMeshDetail->verts = (float*)malloc(sizeof(float) * dverts.size());
	for (int i = 0; i < dverts.size(); i++) {
		navMeshDetail->verts[i] = dverts[i];
	}

	std::vector<unsigned char> tris = detailNavmeshJson.at("tris").get<std::vector<unsigned char>>();
	navMeshDetail->tris = (unsigned char*)malloc(sizeof(unsigned char) * tris.size());
	for (int i = 0; i < tris.size(); i++) {
		navMeshDetail->tris[i] = tris[i];
	}

	navMeshDetail->nmeshes = detailNavmeshJson.at("nmeshes").get<int>();
	navMeshDetail->nverts = detailNavmeshJson.at("nverts").get<int>();
	navMeshDetail->ntris = detailNavmeshJson.at("ntris").get<int>();

	PrepareDetour();
}

void M_Navigation::OnGui()
{
	ImGui::Begin("Navigator");

	ImGui::DragFloat("Cell Size", &navMeshConfig.cs, 0.001f);
	ImGui::DragFloat("Cell Height", &navMeshConfig.ch, 0.001f);
	ImGui::DragFloat("Slope Angle", &navMeshConfig.walkableSlopeAngle, 0.5f);
	ImGui::DragFloat("Walkable Climb", &navMeshConfig.walkableClimb, 0.02f);
	ImGui::DragInt("Walkable Height", &navMeshConfig.walkableHeight, 0.05f);
	ImGui::DragFloat("Walkable Radius", &navMeshConfig.walkableRadius, 0.02f);
	ImGui::DragFloat("Min Region Area", &navMeshConfig.minRegionArea, 0.02f);
	ImGui::DragFloat("Merge Region Area", &navMeshConfig.mergeRegionArea, 0.02f);
	ImGui::DragFloat("Border Size", &navMeshConfig.borderSize, 0.001f);
	ImGui::DragFloat("Max Edge Length", &navMeshConfig.maxEdgeLen, 0.5f);
	ImGui::DragInt("Max Verts Poly", &navMeshConfig.maxVertsPerPoly, 0.2f, 3, 6);
	ImGui::DragFloat("Detail Sample Error", &navMeshConfig.detailSampleMaxError, 0.02f);
	ImGui::DragFloat("Detail Sample Distance", &navMeshConfig.detailSampleDist, 0.02f);
	float3 ex = float3(navMeshConfig.extents[0], navMeshConfig.extents[1], navMeshConfig.extents[2]);
	if (ImGui::DragFloat3("Query Extents", ex.ptr(), 0.02f)) {
		navMeshConfig.extents[0] = ex.x;
		navMeshConfig.extents[1] = ex.y;
		navMeshConfig.extents[2] = ex.z;
	}

	if (ImGui::Button("Bake Navmesh")) {
		ComputeNavmesh();
		PrepareDetour();
	}

	ImGui::Checkbox("Draw Navmesh", &drawNavmesh);

	ImGui::End();
}

bool M_Navigation::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_Navigation::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_Navigation::InspectorDraw()
{
	if (ImGui::CollapsingHeader("Navigator##"))
	{
		ImGui::DragFloat("Cell Size", &navMeshConfig.cs, 0.001f);
		ImGui::DragFloat("Cell Height", &navMeshConfig.ch, 0.001f);
		ImGui::DragFloat("Slope Angle", &navMeshConfig.walkableSlopeAngle, 0.5f);
		ImGui::DragFloat("Walkable Climb", &navMeshConfig.walkableClimb, 0.02f);
		ImGui::DragInt("Walkable Height", &navMeshConfig.walkableHeight, 0.05f);
		ImGui::DragFloat("Walkable Radius", &navMeshConfig.walkableRadius, 0.02f);
		ImGui::DragFloat("Min Region Area", &navMeshConfig.minRegionArea, 0.02f);
		ImGui::DragFloat("Merge Region Area", &navMeshConfig.mergeRegionArea, 0.02f);
		ImGui::DragFloat("Border Size", &navMeshConfig.borderSize, 0.001f);
		ImGui::DragFloat("Max Edge Length", &navMeshConfig.maxEdgeLen, 0.5f);
		ImGui::DragInt("Max Verts Poly", &navMeshConfig.maxVertsPerPoly, 0.2f, 3, 6);
		ImGui::DragFloat("Detail Sample Error", &navMeshConfig.detailSampleMaxError, 0.02f);
		ImGui::DragFloat("Detail Sample Distance", &navMeshConfig.detailSampleDist, 0.02f);
		float3 ex = float3(navMeshConfig.extents[0], navMeshConfig.extents[1], navMeshConfig.extents[2]);
		if (ImGui::DragFloat3("Query Extents", ex.ptr(), 0.02f)) {
			navMeshConfig.extents[0] = ex.x;
			navMeshConfig.extents[1] = ex.y;
			navMeshConfig.extents[2] = ex.z;
		}

		if (ImGui::Button("Bake Navmesh")) {
			ComputeNavmesh();
			PrepareDetour();
		}
	}
	return true;
}
