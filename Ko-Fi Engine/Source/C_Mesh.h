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
	inline void SetPath(const char* path) { mesh->SetAssetPath(path); }
	void SetVertexNormals(bool vertexNormals);
	void SetFaceNormals(bool facesNormals);
	inline void SetRenderMesh(bool renderMesh) { this->renderMesh = renderMesh; }
	
	// Getters
	inline R_Mesh* GetMesh() const { return mesh; }
	inline const char* GetMeshPath() const { mesh->GetAssetPath(); }
	inline float GetSphereRadius() const { return radius; }
	inline bool GetRenderMesh() const { return renderMesh; }
	float3 GetCenterPoint() const { return mesh->localAABB.CenterPoint();  }
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
	
private:
	R_Mesh* mesh = nullptr;
	bool renderMesh = true;
	float time = 0;

	//COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;

	// Bounding sphere
	float radius;

	// Bounding boxes
	AABB aabb;
	OBB obb;
};

#endif // !__COMPONENT_MESH_H__
