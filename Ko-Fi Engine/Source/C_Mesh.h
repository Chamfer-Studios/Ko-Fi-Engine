#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include "MathGeoLib/Geometry/OBB.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "MathGeoLib/Geometry/Sphere.h"
#include "R_Mesh.h"

class C_Transform;
class C_Material;
using Json = nlohmann::json;
typedef unsigned int uint;
class Component;

class C_Mesh : public Component
{
public:
	C_Mesh(GameObject* parent);
	~C_Mesh();

	// Game Loop
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// Serialization
	void Save(Json& json) const override;
	void Load(Json& json) override;

	// SetMesh
	void SetMesh(R_Mesh* mesh);
	void SetPath(const char* path);
	void SetVertexNormals(bool vertexNormals);
	void SetFaceNormals(bool facesNormals);
	
	// Getters
	inline R_Mesh* GetMesh() const { return mesh; }
	const char* GetMeshPath() const;
	inline float GetSphereRadius() const { return radius; }
	inline bool GetRenderMesh() const { return renderMesh; }
	float3 GetCenterPoint() const;
	float3 GetCenterPointInWorldCoords() const;
	uint GetVertices();
	bool GetVertexNormals() const;
	bool GetFaceNormals() const;
	const AABB GetLocalAABB();
	const AABB GetGlobalAABB() const;

	// Mesh Functions
	void GenerateLocalBoundingBox();
	void GenerateGlobalBoundingBox();
	void DrawBoundingBox(const AABB& aabb, const float3& rgb);
	bool InspectorDraw(PanelChooser* chooser);

	inline bool SetDrawAABB() const { return drawAABB; }
	inline void GetDrawAABB(const bool newDrawAABB) { drawAABB = newDrawAABB; }
	
private:
	R_Mesh* mesh = nullptr;
	float time = 0;

	//COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;

	// Bounding sphere
	float radius;

	bool drawAABB = true;
public:
	// Bounding boxes
	AABB aabb;
	OBB obb;
};

#endif // !__COMPONENT_MESH_H__
