#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include "Mesh.h"
#include "par_shapes.h"
#include "MathGeoLib/Geometry/OBB.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "Shader.h"

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

	float3 GetCenterPointInWorldCoords() const;
	inline float GetSphereRadius() const { return radius; }

	bool Start();
	bool Update();
	bool PostUpdate(float dt);
	bool CleanUp();

	//void LoadMesh(const char* path);
	bool InspectorDraw(PanelChooser* chooser);

	uint GetVertices();
	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();
	void Save(Json& json) const override;
	void Load(Json& json) override;

	//public:
	//	ComponentMaterial* materialComponent;

	void SetPath(std::string path);
	void SetVertexNormals(bool vertexNormals);
	void SetFaceNormals(bool facesNormals);

	const char* GetMeshPath();
	bool GetVertexNormals();
	bool GetFaceNormals();
	void GenerateLocalBoundingBox();
	AABB GetLocalAABB();
	void GenerateGlobalBoundingBox();
	AABB GetGlobalAABB();
	void DrawBoundingBox(const AABB& aabb, const float3& rgb);
	bool GetRenderMesh();
	void SetRenderMesh(bool renderMesh);

private:
	//Bounding sphere
	float3 centerPoint = float3::zero;
	float radius;

	Mesh* mesh = nullptr;
	//COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;

	// Bounding boxes
	OBB obb;

	bool renderMesh = true;
	float time = 0;
};

#endif // !__COMPONENT_MESH_H__