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
	bool InspectorDraw();

	void LoadPrimitive(COMPONENT_SUBTYPE subtype);
	void LoadCube();

private:
	const char* path = "";
	std::vector<Mesh*> meshes;
	ComponentMaterial* materialComponent;
};