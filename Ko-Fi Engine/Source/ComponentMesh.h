#pragma once
#include "Component.h"
#include "Mesh.h"
class ComponentMaterial;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* owner,COMPONENT_SUBTYPE subtype);
	ComponentMesh(GameObject* owner,std::string path);
	~ComponentMesh();

	bool Start(const char* path);
	bool PostUpdate();

	void LoadMesh(const char* path);
	bool InspectorDraw(PanelChooser* chooser);

	uint GetVertices();

	ComponentMaterial* materialComponent;

private:
	std::string path = "";
	std::vector<Mesh*> meshes;
	COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;
	// Checkboxes vertex and faces bools to toggle
	bool vertexNormals = false;
	bool facesNormals = false;
};