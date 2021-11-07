#pragma once
#include "Component.h"
#include <vector>

typedef unsigned int uint;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	~ComponentMaterial();
	void LoadTexture(uint& textureID,const char* path);
	bool InspectorDraw();
	void AddTextureId(uint& textureID);
private:
	const char* path = "";
	std::vector<std::reference_wrapper<uint>>textureIds;
};