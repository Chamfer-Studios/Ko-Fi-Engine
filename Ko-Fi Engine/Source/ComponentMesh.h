//#pragma once
#include "Component.h"
#include "Mesh.h"
#include "par_shapes.h"
#include "MathGeoLib/Geometry/OBB.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "Shader.h"
class ComponentTransform;
class ComponentMaterial;

class ComponentMesh : public Component
{
public:
	enum class Shape
	{
		CUBE,
		SPHERE,
		CYLINDER
	};

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

//public:
//	ComponentMaterial* materialComponent;

	void SetPath(std::string path);
	void SetVertexNormals(bool vertexNormals);
	void SetFacesNormals(bool facesNormals);

	GameObject* GetParent();
	std::string GetPath();
	bool GetVertexNormals();
	bool GetFacesNormals();
	void GenerateLocalBoundingBox();
	AABB GetLocalAABB();
	void GenerateGlobalBoundingBox();
	AABB GetGlobalAABB();
	void DrawBoundingBox(const AABB& aabb, const float3& rgb);
	bool GetRenderMesh();
	void SetRenderMesh(bool renderMesh);

	void DrawOutline() const;

private:
	//Bounding sphere
	float3 centerPoint = float3::zero;
	float radius;

	std::string path = "";
	Mesh* mesh = nullptr;
	//COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;
	// Checkboxes vertex and faces bools to toggle
	bool vertexNormals = false;
	bool facesNormals = false;

	// Bounding boxes
	OBB obb;
	AABB aabb;

	bool renderMesh = true;
	float time = 0;
};