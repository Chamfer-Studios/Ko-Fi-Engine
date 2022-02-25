#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <string>
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

class PanelChooser;
using Json = nlohmann::json;
class Shader;


class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	void LoadTexture(std::string path = "");
	bool InspectorDraw(PanelChooser* chooser);
	Material* GetMaterial();
	//Material Handling
	void LoadMaterial(const char* path="");
	Json Save() override;
	void Compile();
	Shader* GetShader();

private:
	void LoadDefaultMaterial();

private:
	Shader* shader = nullptr;
	Material* material = nullptr;
	int currentTextureId=0;
};

#endif // !__COMPONENT_MATERIAL_H__