#pragma once
#include "Component.h"
#include "Texture.h"
#include <vector>
#include <string>
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

typedef unsigned int uint;

class PanelChooser;
#include "Mesh.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	void LoadTexture(const char* path = nullptr);
	bool InspectorDraw(PanelChooser* chooser);
	Texture GetTexture();
	uint GetShader();
	void LoadShader();
private:
	Texture texture;
	uint materialShader = 0;
};