#pragma once
#include "Component.h"
#include "Mesh.h"

class ComponentMaterial;

class ComponentMesh : public Component
{
public:
	ComponentMesh(COMPONENT_SUBTYPE subtype);
	ComponentMesh(const char* path);
	~ComponentMesh();

	bool Start(const char* path);
	bool PostUpdate();

	void LoadMesh(const char* path);
	bool InspectorDraw(PanelChooser* chooser);
	ComponentMaterial* materialComponent;

private:
	const char* path = "";
	std::vector<Mesh*> meshes;
	COMPONENT_SUBTYPE subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;
};