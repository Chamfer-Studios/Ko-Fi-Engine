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

	void LoadTexture(std::string path = "");
	bool InspectorDraw(PanelChooser* chooser);
	Material* GetMaterial();
	
	// Material Handling
	void LoadMaterial(const char* path="");
	Json Save() override;
	void Compile();
	Shader* GetShader();

private:
	void LoadDefaultMaterial();

private:
	Material* material = nullptr;



	Shader* shader = nullptr;

	int currentTextureId = 0;
public:
	//Texture* texture = nullptr;
	std::vector<Texture> textures;
};

#endif // !__COMPONENT_MATERIAL_H__