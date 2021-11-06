#pragma once
#include "Component.h"
#include "Mesh.h"
class ComponentMaterial;
class ComponentMesh : public Component
{
public:
	ComponentMesh(const char* path);
	~ComponentMesh();

	bool Start(const char* path);
	bool PostUpdate();

	void LoadMesh(const char* path);
	bool InspectorDraw();
private:
	const char* path = "";
	std::vector<Mesh*> meshes;
	ComponentMaterial* materialComponent;
};