#pragma once
#include "Component.h"
#include <vector>

typedef unsigned int uint;

class PanelChooser;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	~ComponentMaterial();
	void LoadTextureFromId(uint& textureID,const char* path);
	void LoadTexture(const char* path);
	bool InspectorDraw(PanelChooser* chooser);
	void AddTextureId(uint textureID);
private:
	const char* path = "";
	std::vector<uint>textureIds;
};