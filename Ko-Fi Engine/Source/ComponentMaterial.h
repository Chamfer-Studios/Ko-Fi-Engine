#pragma once
#include "Component.h"
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

	void LoadTextureFromId(uint& textureID, const char* path);
	void LoadTexture(const char* path);
	void LoadDefaultTexture(uint& textureID);
	bool InspectorDraw(PanelChooser* chooser);
	void AddTextures(Texture texture);

private:
	std::string path = "";
	Texture texture;
};