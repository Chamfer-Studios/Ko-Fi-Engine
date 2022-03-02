#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"
#include <vector>
#include <string>

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

using Json = nlohmann::json;

class PanelChooser;
class Material;
class Texture;
class Shader;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	bool InspectorDraw(PanelChooser* chooser);

	Material* GetMaterial() { return material; }
	Shader* GetShader() { return shader; }

	void LoadMaterial(const char* path = "");
	
	void Compile();

	void Save(Json& json) const override;
	void Load(Json& json) override;

private:
	bool LoadDefaultMaterial();

private:
	Material* material = nullptr;

	Shader* shader = nullptr;

	int currentTextureId = 0;
public:
	//Texture* texture = nullptr;
	std::vector<Texture> textures;
};

#endif // !__COMPONENT_MATERIAL_H__