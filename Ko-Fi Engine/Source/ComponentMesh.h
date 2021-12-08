//#pragma once
#include "Component.h"
#include "Mesh.h"
#include "par_shapes.h"
class ComponentMaterial;
//
class ComponentMesh : public Component
{
public:
	enum class Shape
	{
		CUBE,
		SPHERE,
		CYLINDER
	};
//
	ComponentMesh(GameObject* parent);
	//ComponentMesh(GameObject* parent,Shape shape);
	~ComponentMesh();
	//void CopyParMesh(par_shapes_mesh* parMesh);
//
	bool Start(const char* path);
	bool PostUpdate();
	bool CleanUp();
//
//	void LoadMesh(const char* path);
	bool InspectorDraw(PanelChooser* chooser);
//
	uint GetVertices();
	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();
//
//public:
//	ComponentMaterial* materialComponent;
//
private:
	std::string path = "";
	Mesh* mesh = nullptr;
//	//COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;
//	// Checkboxes vertex and faces bools to toggle
	bool vertexNormals = false;
	bool facesNormals = false;
};