#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <string>
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

typedef unsigned int uint;

class PanelChooser;


class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	void LoadDefaultMaterial();
	void LoadTexture(std::string path = "");
	bool InspectorDraw(PanelChooser* chooser);
	Texture GetTexture();
	Material GetMaterial();
	uint GetShader();
	void LoadShader();
private:
	Material material;
	uint materialShader = 0;
};