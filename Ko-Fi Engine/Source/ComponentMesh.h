#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include "Mesh.h"
#include "MathGeoLib/Geometry/OBB.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "MathGeoLib/Geometry/Sphere.h"

class ComponentTransform;
class ComponentMaterial;
using Json = nlohmann::json;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* parent);
	//ComponentMesh(GameObject* parent,Shape shape);
	~ComponentMesh();

	//void CopyParMesh(par_shapes_mesh* parMesh);

	bool Start();
	bool Update();
	bool PostUpdate(float dt);
	bool CleanUp();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	void SetMesh(Mesh* mesh);
	inline Mesh* GetMesh() const { return mesh; }
	//void SetMesh(par_shapes_mesh* parMesh);
	//void CopyParMesh(par_shapes_mesh* parMesh);

	inline void SetPath(const char* path) { mesh->path = path; }
	inline const char* GetMeshPath() const { mesh->path.c_str(); }

	uint GetVertices();

	float3 GetCenterPointInWorldCoords() const;
	inline float GetSphereRadius() const { return radius; }

	void SetVertexNormals(bool vertexNormals);
	bool GetVertexNormals() const;
	void SetFaceNormals(bool facesNormals);
	bool GetFaceNormals() const;

	void GenerateLocalBoundingBox();
	AABB GetLocalAABB();

	void GenerateGlobalBoundingBox();
	AABB GetGlobalAABB();
	
	inline bool GetRenderMesh() const { return renderMesh; }
	inline void SetRenderMesh(bool renderMesh) { this->renderMesh = renderMesh; }

	void DrawBoundingBox(const AABB& aabb, const float3& rgb);

	bool InspectorDraw(PanelChooser* chooser);
	// NEW - Using Space Partitioning
	void GenerateBounds();
	AABB GetGlobalAABB() const;
	void DrawAABB() const;
	// -------------------

private:
	Mesh* mesh = nullptr;

	bool renderMesh = true;
	float time = 0;

	//COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;

	// Bounding sphere
	float3 centerPoint = float3::zero;
	float radius;

	// Bounding boxes
	//OBB obb;
	//AABB aabb;

	bool drawAABB = false;
	OBB obb;
};

#endif // !__COMPONENT_MESH_H__
